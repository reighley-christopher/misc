#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <locale.h>
#include <ncursesw/curses.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "menu-data.h"
#include "menu-term.h"

/*
menu-drive presents assorted functionality as a menu driven system
the first use case will be sending python expressions to the blender server
in general the menu will have to have a list of menu items organized in a tree (will load from a file)

the screen will be split into two panes with a one character high status bar between them

the top pain will always show a menu and the bottom pane will always show a terminal

the modes are therefore :
menu mode : key inputs go to the menu
terminal mode : key inputs go to the terminal

several submodes can be distinguished later :
- groups of menus for different applications, controlled by different threads
- ability to edit terminal commands before they are sent
- ability to parameterize terminal commands 
- do I keep history or do I let the active process do it?
- do I keep a clipboard or do I let the active process do it?

mode changes between terminal and menu mode are done with Ctrl-M
exit with control-C TODO write may need a handler to shut down cleanly

io will probably be asynchronous because I will want the terminal to handle data from a potentially very slow process
the menu structure might change in response to the terminal, either because the output has offered a set of options some input from the terminal 
is being made into a new menu item.

*/

void border_message( WINDOW *border, char message[] )
  {
  mvwaddstr(border, 0, 0, "                       "); 
  mvwaddstr(border, 0, 0, message);
  wrefresh(border);
  }

int term_mode_key_handler(term terminal, char c)
  {
  char onechar[1];
  if( c < 128 && c != ERR && terminal) {
    if(terminal->echo) terminal->display_callback(c , terminal->display_parameter ); 
    onechar[0] = c;
    //TODO need to ensure that the pipe hasn't closed before using it
    write(terminal->writepipe, onechar, 1); //always only write to 1
    }
  return 0;
  }

menu_item hotkey_table[26];
menu_item root_menu;
menu_item current_menu;
term current_term = NULL;

void clear_window(WINDOW *win)
  {
  int h, w, i, j;
  getmaxyx(win, h, w);
  for(i = 0; i < h; i++)
  for(j = 0; j < w; j++)
    mvwaddch(win, i, j, ' ');
  }

void render_menu(WINDOW *top)
  {
  menu_item sploop = current_menu->children;
  clear_window(top);
  wmove(top, 0, 0);
  while(sploop)
    {
    wprintw( top, sploop->name );
    wprintw( top, " ");
    sploop = sploop->sibling;
    }
  wrefresh( top );
  }

void update_hotkey_table(menu_item root)
  {
  int i;
  menu_item sploop = root->children;
  for(i = 0; i < 26; i++) { hotkey_table[i] = NULL; };
  while(sploop)
    {
    hotkey_table[sploop->hotkey-'a'] = sploop;
    sploop = sploop->sibling;
    } 
  }

/* TODO code smell
use of both top, and bottom windows along with writepipe reflects the fact that 
ultimately it is the current menu that determines the state of the current terminal
but putting that intersection here, in the menu's key handler is ugly as sin 
*/
int menu_mode_key_handler(char c, WINDOW *top, WINDOW *bottom)
  {
  menu_item selection = hotkey_table[c-'a'];
  if( c == '-' ) 
    { 
    current_menu = root_menu;
    update_hotkey_table(current_menu);
    render_menu(top); 
    }
  if( c < 'a' || c > 'z' || selection == NULL ) return 1;
  wrefresh(top);
  if( selection->command[0] == '\0' ) 
    {
    if( selection->children == NULL )
      { 
      return -1;
      } else {
      current_menu = selection;
      update_hotkey_table(current_menu);
      render_menu(top);
      }
    }
  if( current_menu == root_menu )
    {
    wprintw(bottom, "[Attempting to start terminal: ");
    wprintw(bottom, selection->command);
    wprintw(bottom, "]");
    current_term = fork_a_thing( c-'a', bottom, selection->command );
    if( selection->children != NULL )
      {
      current_menu = selection;
      update_hotkey_table(current_menu);
      render_menu(top);
      }
    return 0;
    } else {
    write( current_term->writepipe, selection->command, strlen(selection->command) );
    } 
  return 1;
  }

int main(int argc, char *argv[])
  {
  int mode = 1; 
  WINDOW *top, *border, *bottom;
  int maxx, maxy; 
  int winty, winby;
  int i;
  int c, len;
  int forkdex = 's'-'a';
  unsigned char skull[3] = {0xE2, 0x96, 0x81};
  wchar_t skill = 'A';
  char onechar[2];
  for(i = 0; i < 26; i++ ) termset[i] = NULL;
  setlocale(LC_ALL, "");
  initscr();
  start_color();
  onechar[1] = 0;
  getmaxyx(stdscr, maxy, maxx);

  /* two windows of equal size with a 1 char banner between
     of one window with a 1 char banner at the bottom */

  top = newwin(maxy/2, maxx, 0, 0);
  bottom = newwin(maxy/2, maxx, maxy/2+1, 0);
  border = newwin(1, maxx, maxy/2, 0);
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  wbkgd(top, COLOR_PAIR(1) );
  wbkgd(bottom, COLOR_PAIR(1) );
  waddstr(border, "MENU");
  root_menu = initialize_menu("menu.menu");
  current_menu = root_menu;
  render_menu(top);
  update_hotkey_table(root_menu);
  wrefresh(border);
  wrefresh(bottom);
  noecho();
  scrollok(bottom, TRUE);
  cbreak();
  nodelay(bottom, TRUE);
  /* main loop read from forkdex
     write to forkdex 
     
   */
//  current_term = fork_a_thing(forkdex, bottom, "/usr/bin/sqlite3 /home/reighley/Code/misc/c/testdata.sqllite -interactive");
//  current_term = termset[forkdex]; 
  while(mode > -1) 
    {
    c  = wgetch(bottom);
    if( c == 0xc ) {
      mode = mode+1;
      if(mode > 1) mode = 0; 
      switch(mode) {
        case 0 : wrefresh(bottom); border_message( border, "CONSOLE" ); break; 
        case 1 : border_message( border, "MENU" ); break;
        } 
      } else if(c != ERR) switch(mode) { 
      case 0 : mode = term_mode_key_handler(current_term, c) ; break; 
      case 1 : mode = menu_mode_key_handler(c, top, bottom); break; 
      }
    if( current_term ) term_output_buffer(current_term);
    }
  // wgetch(top);
  endwin(); 
  printf("exit\n");
  kill_term(termset[forkdex]);
  //fork_a_thing( forkdex );
  /*this will send exit() to close the session cleanly TODO do better*/
  readwrite( termset[forkdex]->readpipe , termset[forkdex]->writepipe );
  termset[forkdex]->state = 2;
  close( termset[forkdex]->readpipe );
//  getch();
  return 0;
  }
