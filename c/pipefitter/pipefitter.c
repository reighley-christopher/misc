#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
pipefitter command examples :

E 1 somefile
P 1 2
X 

*/

/*TODO this would really benefit from some sort of accounting for all the file handles that need closing*/
/*TODO what about feedback loops?*/

struct execNode
  {
  char *path;
  int in;
  int out;
  int from;
  char tee_out;
  } node_buffer[16];

struct tees
  {
  int in;
  int outs[16];
  int numouts;
  pid_t pid;
  } tee_buffer[8];

int node_count = 0;
int tee_count = 0;

/*reminder on use of pipe : we write to [1] and read from [0]*/

void tee_create(struct execNode *node)
  {
  int io[2];
  node->tee_out = tee_count;
  pipe(io);
  tee_buffer[tee_count].in = io[0];
  tee_buffer[tee_count].outs[0] = node->out;
  tee_buffer[tee_count].numouts = 1;
  node->out = io[1];
  tee_count = tee_count + 1;
  }

void tee_add(struct tees *tee, int write_pipe)
  {
  tee->outs[tee->numouts] = write_pipe;
  tee->numouts = tee->numouts + 1; 
  }

void E(int label, char *path)
  {
  node_count = node_count+1;
  node_buffer[label].path = path;
  node_buffer[label].in = 0;
  node_buffer[label].out = 1;
  node_buffer[label].from = -1;
  node_buffer[label].tee_out = -1;
  }

void P(int from, int to)
  {
  /*
  creates a new pipe to connect from and to.
  if the output of from is already destined for a source, 
  create a tee or use the one already created;
  */
  int io[2];
  /*TODO 6 very different cases are represented here, unit test this in more detail*/
  if(node_buffer[to].in != 0)
    {
    /*oh look we already have a file handle for this pipe, lets just use it*/
    if(node_buffer[from].out != 1)
      {
      if(node_buffer[from].tee_out < 0)
        {
        tee_create(node_buffer + from);
        }
      tee_add(tee_buffer + node_buffer[from].tee_out, node_buffer[to].from);
      } else {
      node_buffer[from].out = node_buffer[to].from;
      }
    } else {
    pipe(io);
    node_buffer[to].in = io[0];
    node_buffer[to].from = io[1];
    if(node_buffer[from].out != 1)
      {
      if(node_buffer[from].tee_out < 0)
        {
        /*create new tee*/
        tee_create(node_buffer + from);
        }
      /*add to existing tee*/
      tee_add(tee_buffer + node_buffer[from].tee_out, io[1]);
      } else {
      /*first output pipe from this node, no tee necessary*/
      node_buffer[from].out = io[1];
      }
    }
  }

void slurp_tee(struct tees *tee)
  {
  char c;
  int in = tee->in;
  int numouts = tee->numouts;
  int *outs = tee->outs;
  int i;
  char buffer[256];
  int length;
  char s[3] = "-;\0";
  length = read(in, buffer, 256);
  while(length > 0)
    {
    for(i = 0; i < numouts; i++)
      {
      s[0] = '0' + i;
      /*TODO I think this code could deadlock if one of the buffers fills up because downstream refuses to read*/
      write(outs[i], s, 2 );
      write(outs[i], buffer, length);
      }
    length = read(in, buffer, 256);
    }
  for(i = 0; i < numouts; i++)
    {
    close(outs[i]);
    }
  exit(0);
  }

void close_filehandles()
  {
  int j;
  for(j = 0; j < node_count; j++)
    if(node_buffer[j].out != 1)
      close(node_buffer[j].out);
  }
 
void fork_tee(struct tees *tee)
  {
  pid_t f = fork();
  int i;
  close_filehandles();
  if(!f) {slurp_tee(tee);} else {tee->pid = f;};
  /*close the tee outputs in the master process*/
  for(i = 0; i < tee->numouts; i++)
    close(tee->outs[i]);
  }

void start_tees()
  {
  int i;
  for(i = 0; i < tee_count; i++)
    {
    fork_tee(tee_buffer + i);
    }
  }

void fork_exec(struct execNode *exec, int i)
  {
  /*start a command with its own stdin an stdout*/
  int j, k;
  pid_t f = fork();
  if(!f)
    {
    /*close all the outbound file handles that don't belong to me*/
    /*TODO this seems sloppy, duplicating all these open file handles like this*/
    for(j = 0; j < node_count; j++)
      if(node_buffer[j].out != exec->out && node_buffer[j].out != 1)
        close(node_buffer[j].out);
    for(j = 0; j < tee_count; j++)
      for(k = 0; k < tee_buffer[j].numouts; k++)
        close(tee_buffer[j].outs[k]);
    if(exec->in != 0) 
      {
      close(0);
      dup(exec->in);
      }
    if(exec->out != 1)
      {
      close(1);
      dup(exec->out);
      close(exec->out);
      }
    system(exec->path);
    exit(0);
    }  
  }

void start_execs()
  {
  int i;
  for(i = 0; i < node_count; i++)
    {
    fork_exec(node_buffer + i, i);
    }
  }

int main( int argc, char *argv[] )
  {
  int command = 'A';
  int state = 0;
  int accumulator = 0;
  int from, to;
  char c = fgetc(stdin);
  char path_buffer[256];
  char *name_ptr = path_buffer;
  while(command != 'X' && c != EOF)
    {
    switch(state)
      {
      case(0): 
        command = c;
        state = 1; 
        break; /*state 0 read command*/
      case(1): 
        state = 2;
        break;
      case(2):
        if(c >= '0' && c <= '9') 
          {
          accumulator = accumulator*10;
          accumulator = accumulator + c - '0';
          } else {
          from = accumulator;
          accumulator = 0;
          state = 3;
          }
        break;
      case(3):
        switch(command)
          {
          case('E'):
            /*the character is part of a path*/
            if(c != '\n')
              {
              name_ptr[accumulator] = c;
              accumulator = accumulator + 1;
              } else {
              name_ptr[accumulator] = '\0';
              E(from, name_ptr);
              name_ptr = name_ptr + accumulator + 1;
              accumulator = 0;
              state = 0;
              }            
            break;
          case('P'):
            if(c >= '0' && c <= '9') 
              {
              accumulator = accumulator*10;
              accumulator = accumulator + c - '0';
              } else {
              to = accumulator;
              accumulator = 0;
              if(c == '\n') {P(from, to); state = 0;} else {state=4;}
              }
            break;
          }
        break;
      case(4):
        if(c == '\n') 
          {
          P(from, to); 
          state = 0;
          }
        break;
      }
    c = fgetc(stdin);
    }
  start_execs();
  start_tees();
  close_filehandles();
  }


