#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include "bytebuffer.h"
#include <errno.h>

/*
faninout command examples :
l /usr/lib/module.so
o fifo1
i fifo2
d fifo1
s +a
e /usr/bin/executable 
?
x
*/

/*
s enables or disables options
options should be :
insert 0xE0 record separator on inputs 
synchronous, the fifos take turns
insert 0x1E record spearator on output (if synchronous is set use 0x1D group separator between batches)
*/

completer_module *get_yajl_completer_module();

char name_table[1024];
int name_max = 0;

struct inputfifo
  {
  char *name;
  pthread_t thread;
  } fanin_buffer[16];

int fanin_count = 0;

struct outputfifo
  {
  char *name;
  int fd;
  } fanout_buffer[16];

int fanout_count = 0;

struct execNode
  {
  char *path;
  int in;
  int out;
  int from;
  char tee_out;
  pid_t pid;
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
  printf("create %s\n", path);
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
      /*TODO this seems to be the case when a fifo is involved  */
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

void print_read_errno()
  {
  switch(errno)
    {
    case EAGAIN : printf("EAGAIN\n"); break;
    case EBADF : printf("EBADF\n"); break;
    case EFAULT : printf("EFAULT\n"); break;
    case EINTR : printf("EINTR\n"); break;
    case EINVAL : printf("EINVAL\n"); break;
    case EIO : printf("EIO\n"); break;
    case EISDIR : printf("EISDIR\n"); break;
    default : printf("unidentified errno\n");
    }; 
  }

void print_write_errno()
  {
  switch(errno)
    {
    case EAGAIN : printf("EAGAIN\n"); break;
    case EBADF : printf("EBADF\n"); break;
    case EDESTADDRREQ : printf("EDESTADDRREQ\n"); break;
    case EDQUOT : printf("EDQUOT\n"); break;
    case EFAULT : printf("EFAULT\n"); break;
    case EFBIG : printf("EFBIG\n"); break;
    case EINTR : printf("EINTR\n"); break;
    case EINVAL : printf("EINVAL\n"); break;
    case EIO : printf("EIO\n"); break;
    case ENOSPC : printf("ENOSPC\n"); break;
    case EPERM : printf("EPERM\n"); break;
    case EPIPE : printf("EPIPE\n"); break;
    default : printf("unidentified errno\n");
    }
  }

void print_open_errno()
  {
  switch(errno)
    {
    case EACCES : printf("EACCES\n"); break;
    case EBUSY : printf("EBUSY\n"); break;
    case EDQUOT : printf("EDQUOT\n"); break;
    case EEXIST : printf("EEXIST\n"); break;
    case EFAULT : printf("EFAULT\n"); break;
    case EFBIG : printf("EFBIG\n"); break;
    case EINTR : printf("EINTR\n"); break;
    case EINVAL : printf("EINVAL\n"); break;
    case EISDIR : printf("EISDIR\n"); break;
    case ELOOP : printf("ELOOP\n"); break;
    case EMFILE : printf("EMFILE\n"); break;
    case ENAMETOOLONG : printf("ENAMETOOLONG\n"); break;
    case ENFILE : printf("ENFILE\n"); break;
    case ENODEV : printf("ENODEV\n"); break;
    case ENOENT : printf("ENOENT\n"); break;
    case ENOMEM : printf("ENOMEM\n"); break;
    case ENOSPC : printf("ENOSPC\n"); break;
    case ENOTDIR : printf("ENOTDIR\n"); break;
    case ENXIO : printf("ENXIO\n"); break;
    case EOPNOTSUPP : printf("EOPNOTSUPP\n"); break;
    case EOVERFLOW : printf("EOVERFLOW\n"); break;
    case EPERM : printf("EPERM\n"); break;
    case EROFS : printf("EROFS\n"); break;
    case ETXTBSY : printf("ETXTBSYM\n"); break;
    case EWOULDBLOCK : printf("EWOULDBLOCK\n"); break;
    default : printf("unrecognized errno value\n"); 
    }
  }

void start_tees()
  {
  int i;
  for(i = 0; i < tee_count; i++)
    {
    fork_tee(tee_buffer + i);
    }
  }

pid_t fork_exec2(char *path, int inp, int outp)
  {
  pid_t f = fork();
  if(!f)
    {
    /*I am the child process, I want new input and output*/
    if( inp != 0 )
      {
      close(0);
      dup(inp);
      }    
    if( outp != 1 )
      {
      close(1);
      dup(outp);
      }
    return f;
    } else {
    /*I am the parent process, I am just going to return the pid of the child*/ 
    return f;
    }
  }

void fork_exec(struct execNode *exec, int i)
  {
  /*start a command with its own stdin an stdout*/
  int j, k;
  pid_t f = fork();
  //int f = fork();
  printf("forking %s %d\n", exec->path, f);
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
    printf("executing %s\n", exec->path);
    if(exec->out != 1)
      {
      close(1);
      dup(exec->out); 
      /*this works because the dup call will use the lowest numbered unused descriptor 
        (so it will be 1, ie stdout). it has been a long time since I have been this mad at unix
      */
      close(exec->out);
      }
    system(exec->path);
    printf("%s terminated\n", exec->path);
    exit(0);
    } else { exec->pid = f; } 
  }

pid_t current_process = 0;
int write_pipe, read_pipe = 0;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

/*the most general case of complete will need to see the whole buffer but for our purposes I just need the last block*/
int complete(void *d, char *data, int length)
  {
  /*stub complete flushes on a newline, but wait there might be some left over??*/
  int i;
  for(i = 0 ; i < length; i++)
    {
    //printf("%c", data[i]);
    if(data[i] == '\n') 
      {
      printf("complete read %d\n", i+1);
      return i+1;
      }
    }
  return -1;
  } 

void *input_pthread_loop(void *d)
  {
  /*d contains a pointer to the inputfifo record I will want to copy it into the local stack because it may move*/
  struct inputfifo inp = *( (struct inputfifo *)d )  ;
  bytebuffer buffer;
  bytebuffer_iter iter;
  unsigned char *data;
  unsigned int length;
  int fifo;
  int mark = -1;
  
  /*TODO how to ensure that the bytebuffer is cleaned up when the thread stops*/
  bytebuffer_init(&buffer);
  bytebuffer_set_completer(&buffer, get_yajl_completer_module()); 
  /*I will open the fifo, I will loop waiting for data, as data comes I will read it into a buffer
    it is in blocking mode because I will only care to read it if there is data to be read*/
  fifo = open( inp.name, O_RDONLY ); 
  if(fifo < 0)
    {
    print_open_errno();
    return 0;
    } 
  
  while(1) /* TODO what should happen when the file descriptor closes? (returns 0) */ 
    {
    /*if the buffer is full I will need to allocate more buffer using bytebuffer_append_start and bytebuffer_append_trim*/
    bytebuffer_append_start( &buffer, &data, &length);
    length = read(fifo, data, length);
    if(length < 0)
      {
      print_read_errno(); /*TODO this will print undefined errno at eof because length will be 0, what I should do is reopen the fifo*/
      return 0;
      }
  
    /*for bytebuffer_mark_complete to work it needs to be called here, before bytebuffer_append_trim*/
    /*TODO there is a possibility of an error here for large lenghts, since length is unsigned but complete needs to be able to return -1*/
    //if( ( mark = complete(0, data, length) ) > 0 ) bytebuffer_set_complete_mark(&buffer, mark);
    bytebuffer_append_trim(&buffer, length);
 
    /*when the record is complete I will then claim the mutex and write to the output using write_record, bytebuffer_iter_init and
    bytebuffer_iter_next */
    if( bytebuffer_record_available(&buffer) ) /*TODO stubbing out complete, what does the general signature look like*/
      {
      printf("faninout:366 record available\n");
      bytebuffer_iter_init(&iter, &buffer);
      /*claim mutex here*/
      pthread_mutex_lock(&write_mutex);
      while( bytebuffer_iter_next( &iter, &data, &length ) ) 
        {
        write(write_pipe, data, length);
        printf("faninout.c:369 sending %d characters to process:", length);  
        write(0, data, length); write(0, "[]\n", 3 );
        }
      /*release mutex here*/
      pthread_mutex_unlock(&write_mutex);
      bytebuffer_rotate_record(&buffer);
      mark = -1; 
      }
    } 
  }

void check_open(struct outputfifo *out)
  {
  /*if the fifo is open leave it alone, if it is not, try to open it if the ENXIO error occurs swallow it, otherwise print the
    error*/
  if(out->fd == 0 )
    {
    out->fd = open(out->name, O_NONBLOCK|O_WRONLY);
    if(out->fd < 0)
      {
      out->fd = 0;
      if(errno != ENXIO) print_open_errno();
      }
    } 
  }



/*there only needs to be one output loop thread, because the records are read one at a time*/
void *output_inner_loop(void *d)
  {
  bytebuffer buffer;
  bytebuffer_iter iter;
  unsigned char *data;
  unsigned int length;
  int err;
  int i, out_index;
  bytebuffer_init(&buffer);
  bytebuffer_set_completer(&buffer, get_yajl_completer_module());
  while(1)
    { /*TODO I should sleep the thread if there is no read pipe, and starting the executable should wake the thread*/ if(read_pipe) { bytebuffer_append_start(&buffer, &data, &length); length = read(read_pipe, data, length); for(i = 0; i < length; i++) printf("%c.", data[i]); bytebuffer_append_trim(&buffer, length);
      if( bytebuffer_record_available(&buffer) )
        {
        for(out_index = 0; out_index < fanout_count; out_index ++)
          {
          bytebuffer_iter_init(&iter, &buffer);
          check_open(fanout_buffer + out_index);
          while(fanout_buffer[out_index].fd && bytebuffer_iter_next(&iter, &data, &length))
            {
            /*write until the buffer is empty or I get a write error, write error will close the output and drop the rest of the 
              record, if the write is incomplete oh well*/
            //TODO handle incomplete writes
            printf("emmitting %d bytes\n", length);
            err = write(fanout_buffer[out_index].fd, data, length );
            if(err == -1) 
              {
              print_write_errno();
              fanout_buffer[out_index].fd = 0;
              }
            }
          } 
        bytebuffer_rotate_record(&buffer); 
        }  
      }
  /*TODO :*/
  /*tell this thread to stop when the main process terminates*/
  /*transform the output according to a rule*/
  /*I decide which outputs deserve to see this record*/
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

void add_fanin(char *name)
  {
  fanin_buffer[fanin_count].name = name; 
  fanin_count = fanin_count + 1;
  pthread_create(&(fanin_buffer[fanin_count].thread), NULL, input_pthread_loop, &fanin_buffer[fanin_count-1] ); 
  }

void add_fanout(char *name)
  {
  //TODO what if file not found
  //TODO how to keep fifo from blocking
  //open will fail with ENXIO if the other end has not opened first 
  fanout_buffer[fanout_count].name = name;
  check_open(fanout_buffer+fanout_count);
  fanout_count = fanout_count + 1; 
  }

char *locate_name(char *name)
  {
  int i = 0;
  char *candidate = name_table;
  while(i < name_max)
    {
    if(name_table[i] == name[i - ( candidate - name_table ) ])
      {
      if( name_table[i] == '\0' ) { return candidate; };
      } else {
      while(i < name_max && name_table[i] != '\0' ) i = i+1;
      candidate = name_table + i + 1;
      }
    i = i+1; 
    }
  return 0;
  }

void delete(char *name)
  {
  int i = 0;
  char *n = locate_name(name);

  pthread_cancel(fanin_buffer[i].thread);
  for(i = 0; i < fanin_count; i++)
    {
    if(n == fanin_buffer[i].name)
      {
      /*copy the last element into this one*/
      fanin_buffer[i].name = fanin_buffer[fanin_count-1].name;
      fanin_buffer[i].thread = fanin_buffer[fanin_count-1].thread;
      fanin_count = fanin_count-1; 
      return; 
      } 
    }

  for(i = 0; i < fanout_count; i++)
    {
    if(n == fanout_buffer[i].name)
      {
      /*copy the last element into this one*/
      fanout_buffer[i].name = fanout_buffer[fanout_count-1].name;
      fanout_count = fanout_count-1; 
      return; 
      } 
    }

  }

void execute(char *name)
  {
  pid_t f;
  int upstream[2]; 
  int downstream[2];
  /*pipe reads from 0 writes to 1*/
  pipe(upstream);
  pipe(downstream); 
  if( current_process ) { kill( current_process, SIGTERM ); } //TODO if current_process doesn't die we will never see it again but it will be there
  f = fork_exec2(name, upstream[0], downstream[1]);
  write_pipe = upstream[1];
  read_pipe = downstream[0]; 
  if( !f )
    {
    system(name); //TODO I am going to reuse name, does system need it?
    exit(0);
    } else {
    current_process = f;
    }
   
  }

void questionmark()
  {
  int i = 0;
  printf("in:\n", fanin_count, name_max);
  for(i=0; i < fanin_count; i++) {
    printf("\t%s\n", fanin_buffer[i].name);
    }
  printf("out:\n", name_max);
  for(i=0; i < fanout_count; i++) {
    printf("\t%s\n", fanout_buffer[i].name);
    }
  }

void evaluate(int command, char *value) 
  {
  switch(command)
    {
    case('i'):
      name_max = name_max + 1; /*keep string*/
      add_fanin(value);
      break;
    case('o'):
      name_max = name_max + 1; /*keep string*/
      add_fanout(value);
      break;
    case('d'):
      name_max = value - name_table; /*discard string*/
      delete(value);
      break;
    case('e'):
      name_max = value - name_table; /*discard string*/
      execute(value);
      break;
    case('?'):
      questionmark();
      name_max = value - name_table; /*discard string*/
      break;
    default:
      name_max = value - name_table; /*discard string*/
      printf("error condition\n"); 
    }
  }

pthread_t output_thread;
void cleanup()
  {
  int i;
  pthread_kill(output_thread, SIGKILL);
  for(i = 0; i < fanin_count; i++)
    {
    pthread_kill(fanin_buffer[i].thread, SIGKILL);
    } 
  if( current_process ) { kill( current_process, SIGKILL ); }
  }

void sighandler(int sig)
  {
  if(sig == SIGPIPE)
    {
    printf("signal handler : SIGPIPE\n");
    }
  if(sig == SIGTERM)
    {
    cleanup();
    exit(0);
    }
  }

int main( int argc, char *argv[] )
  {
  int command = 'A';
  int state = 0;
  int accumulator = 0;
  int from, to;
  char c = fgetc(stdin);
  char path_buffer[256]; //TODO expand this buffer?
  char *path_ptr = name_table + name_max;
  pthread_create(&output_thread, NULL, output_inner_loop, NULL);
  signal(SIGPIPE, sighandler);
  signal(SIGTERM, sighandler);
 while(command != 'x')
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
        /*the character is part of a path*/
        if( c == EOF ) state = 3;
        if(c != '\n')
          {
          path_ptr[accumulator] = c;
          accumulator = accumulator + 1;
          } else {
          path_ptr[accumulator] = '\0';
          name_max = name_max + accumulator;
          evaluate(command, path_ptr);
          path_ptr = name_table + name_max;
          accumulator = 0;
          state = 0;
          }            
        break;
      case(3): break; //we don't need to do anything else
      }
    c = fgetc(stdin);
    }
  cleanup(); 
//  start_execs();
//  start_tees();
//  close_filehandles();
  }


