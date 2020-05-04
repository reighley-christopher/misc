#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncursesw/curses.h>
#include <pthread.h>
#include "menu-term.h"

#include <sched.h>

void *thread_handler( void *t );
void curses_display_callback( char c, void *p );

/*
void fork_a_thingx( int index , WINDOW *start)
  {
  int downpipe[2];
  int uppipe[2];
  int child_id;
  pipe(downpipe);
  pipe(uppipe);
  child_id = fork();
  if(child_id == 0)
    {
    close(downpipe[1]);
    close(uppipe[0]);
    create_processx(downpipe[0], uppipe[1], "/usr/bin/sqlite3 /home/reighley/Code/misc/c/testdata.sqllite -interactive");
    }
  termset[index]->state = 1;
  termset[index]->writepipe = downpipe[1];  
  termset[index]->readpipe = uppipe[0];
  close(uppipe[1]);
  close(downpipe[0]);
  term_set_display_callback( termset[index], curses_display_callback, (void *)start );
  pthread_create( &(termset[index]->threadid), NULL, thread_handler, (void *)termset[index] ); 
  }
*/
/*stuff copied from menu-term.c goes above this line*/

int term_mode_key_handler(term terminal, char c)
  {
  char onechar[1];
  if( c < 128 && c != ERR) {
    if(terminal->echo) terminal->display_callback(c , terminal->display_parameter ); 
    onechar[0] = c;
    //TODO need to ensure that the pipe hasn't closed before using it
    write(terminal->writepipe, onechar, 1); //always only write to 1   
    }
  return 0;
  }

void term_print_buffer(term terminal)
  {
  int i;
  printf("[");
  for(i = 0; i < terminal->buffer_offset; i++)
    {
    printf("%c", terminal->buffer[i]);
    }
  printf("]\n");
  }

int main(int argc, char *argv[])
  {
  char c = '\0';
  int i;

  /*
  create_processx(0, 1, "/usr/bin/sqlite3 /home/reighley/Code/misc/c/testdata.sqllite -interactive");
  return 0;
  */

  termset[0] = new_term();
  initscr();
  cbreak();
  noecho(); 
  nodelay( stdscr, TRUE );
  fork_a_thing(0, stdscr, "/usr/bin/sqlite3 /home/reighley/Code/misc/c/testdata.sqllite -interactive");
  while(1)
    {
    c = wgetch(stdscr);
    if( c != ERR ) term_mode_key_handler(termset[0], c);
    term_output_buffer(termset[0]); 
    wrefresh(stdscr); 
    }

  endwin();
  getchar();
  term_print_buffer(termset[0]);
  readwrite( termset[0]->readpipe , termset[0]->writepipe );
  getchar();
  term_print_buffer(termset[0]);
  termset[0]->state = 2;
  close( termset[0]->readpipe );
  } 
