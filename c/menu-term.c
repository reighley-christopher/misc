#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <ncursesw/curses.h>
#include <stdlib.h>
#include "menu-term.h"

/* these should no longer be necessary, have been moved inside the term struct */
//pthread_mutex_t buffer_mutex;
//char buffer[BUFFER_SIZE]; /*buffering the input makes curses rendering worse!! WTF*/
//int buffer_offset = 0;

void *thread_handler( void *t )
  {
  term terminal = (term)t;
  int len;
  int i;
  char onechar[1];
  while( terminal->state != 2 ) 
    {
    len = read(terminal->readpipe, onechar, 1);
    if( len > 0 )
      {
      pthread_mutex_lock( &(terminal->buffer_mutex) );
      terminal->buffer[terminal->buffer_offset] = onechar[0];
      terminal->buffer_offset = terminal->buffer_offset + 1;
      pthread_mutex_unlock( &(terminal->buffer_mutex) );
      }
    }
  return NULL;
  }

void create_process(int readpipe, int writepipe, char *command)
  {
  char *list[1024];
  char buffer[1024];
  int buffer_index = 0;
  int list_index = 0;
  int i = 0;;
  buffer[0] = command[0];
  list[0] = buffer;
  while(command[i] != '\0')
    {
    if( command[i] == ' ' )
      {
      list_index = list_index + 1;
      list[list_index] = buffer+i+1;
      buffer[i] = '\0';
      } else {
      buffer[i] = command[i];
      }
    i = i+1;
    list[i] = NULL;
    } 
    list_index = list_index + 1;
    list[list_index = 0];
  /*create the process "/usr/bin/sqlite3 /home/reighley/test.sqlite3 -interactive" */
  dup2(readpipe, 0);
  dup2(writepipe, 1); /*setting writepipe to 1 will be a noop*/
  execv(list[0], list);
  //execl("/usr/bin/sqlite3", "/usr/bin/sqlite3", "/home/reighley/Code/misc/c/testdata.sqllite", "-interactive", (char *)NULL );
  //execl("/usr/bin/socat", "/usr/bin/socat", "UNIX:/home/reighley/test_socket", "STDIO", (char *)NULL );
  //execl("/usr/bin/bash", "/usr/bin/bash -i", (char *)NULL );
  }


void create_processx(int readpipe, int writepipe)
  {
  /*create the process "/usr/bin/socat UNIX:/home/reighley/test_socket STDIO" */
  dup2(readpipe, 0);
  dup2(writepipe, 1); /*setting writepipe to 1 will be a noop*/
  execl("/usr/bin/socat", "/usr/bin/socat", "UNIX:/home/reighley/test_socket", "STDIO", (char *)NULL );
  }

/*
readpipe will be pipe[0] in a pipe pair, the read end
writepipe will be the pipe[1] in a pipe pair, the write end
*/
void readwrite( int readpipe, int writepipe )
  {
  char buffer[100];
  int len;
  int i;
  write(writepipe, "exit()\n", 7); //always only write to 1
  //sleep(1); 
  len = read(readpipe, buffer, 10);
  printf("%d %s\n", len, buffer);
  for(i = 0; i < len; i++)
    {
    printf("%d %x\n", i, buffer[i]);
    }
  }

term new_term()
  {
  term ret = (term)malloc(sizeof(struct _term));
  pthread_mutex_init(&(ret->buffer_mutex), NULL); 
  ret->echo = 1;
  ret->buffer_offset = 0;
  return ret;
  }

void term_set_display_callback( term terminal, void (*display)(char c, void *p), void *parameter )
  {
  terminal->display_callback = display;
  terminal->display_parameter = parameter;
  }

void kill_term( term target )
  {
  pthread_mutex_destroy(&(target->buffer_mutex));
  }

void curses_display_callback( char c, void *p )
  {
  WINDOW *w = (WINDOW *)p;
  waddch( w, c );
  }

void term_output_buffer(term terminal)
  {
  int i;
  pthread_mutex_lock( &( terminal->buffer_mutex ) );
  for(i = 0; i < terminal->buffer_offset; i++)
    {
    terminal->display_callback( terminal->buffer[i], terminal->display_parameter );
    }
  terminal->buffer_offset = 0;
  pthread_mutex_unlock( &( terminal->buffer_mutex ) );
  }

/*TODO code smell
first the interaction between the terminal and the curses window (we have model and view but no controller)
second the fact that someone needs to be responsible for the table which is declared in and used in menu-term.c
but will have to be initialized in the main loop. 
*/

term fork_a_thing( int index , WINDOW *start, char *command)
  {
  int downpipe[2];
  int uppipe[2];
  int child_id;
  if( termset[index] != NULL ) return termset[index]; 
  termset[index] = new_term();
 
  pipe(downpipe);
  pipe(uppipe);
  child_id = fork();
  if(child_id == 0)
    {
    close(downpipe[1]);
    close(uppipe[0]);
    create_process(downpipe[0], uppipe[1], command );
    }
  termset[index]->state = 1;
  termset[index]->writepipe = downpipe[1];  
  termset[index]->readpipe = uppipe[0];
  close(uppipe[1]);
  close(downpipe[0]);
  term_set_display_callback( termset[index], curses_display_callback, (void *)start );
  pthread_create( &(termset[index]->threadid), NULL, thread_handler, (void *)termset[index] );

  if( term_test_echo(termset[index]) ) 
    {
    term_message(termset[index], "[ECHO OFF]");
    termset[index]->echo = 0;
    } else term_message(termset[index], "[ECHO ON]");
  return termset[index];
  }

int term_message(term terminal, char *message)
  {
  int i = 0;
  while( message[i] != '\0' )
    {
    terminal->display_callback( message[i], terminal->display_parameter );
    i = i + 1;
    } 
  }

int term_test_echo(term terminal)
  {
  /*sends return, yields, drains the buffer, sends a sequence of 8 spaces and a tab and then checks the buffer again.
    if there are 8 spaces at the end of the output then the pipe is echoing
   */
  int i;
  write(terminal->writepipe, "\n", 1);
  for(i = 0; i < 10000; i++) sched_yield(); 
  terminal->buffer_offset = 0;
  write(terminal->writepipe, "        ", 8);
  for(i = 0; i < 100000; i++) sched_yield();
  term_message(terminal, "[TEST TERMINAL MESSAGE]");
  if( terminal->buffer_offset >= 8 ) term_message( terminal, "[OFFSET >= 8]" ); else return 0;
  if( terminal->buffer[terminal->buffer_offset-1] == ' ' ) term_message( terminal, "[SPACE DETECTED]" ); else return 0;  
  for(i = 0; i < 8; i++) if(terminal->buffer[i] != ' ') return 0; 
  return 1;
  }

