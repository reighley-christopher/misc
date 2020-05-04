#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "menu-data.h"

menu_item new_menu_item( char *name, char hotkey, char *command )
  {
  menu_item ret = (menu_item)malloc(sizeof(struct _menu_item));
  ret->name = name; 
  ret->hotkey = hotkey;
  ret->command = command;
  ret->shutdown = NULL;
  ret->parent = NULL;
  ret->children = NULL;
  ret->sibling = NULL;
  return ret;
  }

void append_menu_item( menu_item parent, menu_item child )
  {
  child->parent = parent;
  child->sibling = parent->children;
  parent->children = child; 
  }

menu_item xinitialize_menu(char *menu_path)
  {
  menu_item root_menu = new_menu_item("", '\0', "");
  menu_item exit_item = new_menu_item("eXit", 'x', "");
  menu_item hello_item = new_menu_item("Hello", 'h', "print(\"hello world\")\n");
  menu_item sub_menu = new_menu_item("Submenu", 's', "");
  menu_item hello2_item = new_menu_item("Hello2", 'h', "print(\"HELLO WORLD\")\n");
  append_menu_item(root_menu, exit_item);
  append_menu_item(root_menu, hello_item);
  append_menu_item(root_menu, sub_menu);
  append_menu_item(sub_menu, hello2_item);
  return root_menu;
  }

void menu_normalize_names( menu_item root )
  {
  int hotkey_set = 0;
  int i = 0;
  int len = strlen(root->name);
  char c;
  menu_item sploop;
  if( !root ) return;
  for(i = 0; i < len; i++)
    {
    if( root->name[i] >= 'A' && root->name[i] <= 'Z' ) root->name[i] = root->name[i]-'A'+'a'; /*lower case by default*/
    if( root->name[i] == root->hotkey && !hotkey_set )
      {
      root->name[i] = root->name[i] - 'a' + 'A';
      hotkey_set = 1; 
      } 
    }
  sploop = root->children;
  while(sploop)
    {
    menu_normalize_names( sploop );
    sploop = sploop->sibling;
    }
  }

/*file format for importing the menu*/
//TODO this should be a seperate file so I can easily change formats later
enum read_state
  {
  HEAD = 0,
  ATTR = 1,
  BODY = 2,
  INC = 3,
  END = 4,
  ERR = 5,
  SKIP = 6 
  };

enum attribute
  {
  NONE = 0,
  SHUTDOWN = 1
  };

void process_one_line( FILE *fp, char **string, enum read_state *state )
  {
  char buffer[255];
  int index = 0;
  int c;
  c = fgetc( fp );
  switch(c)
    {
    case EOF: *state = END; return;
    case '-': c = fgetc( fp ); *state = BODY; break;
    case '%': c = fgetc( fp ); *state = ATTR; break;
    default : *state = HEAD; 
    }
  while( c != '\n' && c != EOF)
    {
    buffer[index] = c; 
    c = fgetc( fp );
    index = index+1;
    }
  buffer[index] = '\0';
  if( index == 0 ) { *state = SKIP; return; }
  *string = malloc(index * sizeof(char));
  strcpy(*string, buffer); 
  }

void body_append( char **original, char *string )
  {
  char *newstring;
  if(*original == NULL) { *original = string; return; } 
  newstring = (char *)malloc( (strlen(string) + strlen(*original) + 2 ) * sizeof(char) ); // +2 for \n and \0
  strcpy( newstring, *original );
  strcpy( newstring + strlen(*original), "\n");
  strcpy( newstring + strlen(*original) + 1, string); 
  free(*original);
  free(string);
  *original = newstring;
  }

menu_item locate( menu_item root, char* path )
  {
  /*TODO trailing spaces screw up the path, need to handle this or it will be a pain someday*/
  char name[255];
  char *newname;
  int name_length = 0;
  int index = 0;
  menu_item sploop = root;
  while(path[index] != '\0')
    {
    while(path[index] != '\0' && path[index] != '>') { name[name_length] = path[index]; index++; name_length++; }
    if(path[index] == '>') index = index + 1;
    name[name_length]='\0';
    name_length = 0;
    if(sploop->children == NULL)
      {
      newname = (char *)malloc(sizeof(char)*( strlen(name) + 1 ) );
      strcpy(newname, name);
      sploop->children = new_menu_item( newname, '\0', NULL ); 
      }
    sploop = sploop->children;
    while( strcmp(name, sploop->name) )
      {
      if(sploop->sibling == NULL)
        {
        newname = (char *)malloc(sizeof(char)*( strlen(name) + 1 ) );
        strcpy(newname, name);
        sploop->sibling = new_menu_item( newname, '\0', NULL );
        sploop->sibling->parent = sploop->parent; 
        } 
      sploop = sploop->sibling;
      }
    } 
  return sploop; 
  }

void assign_hotkeys(menu_item root)
  {
  menu_item sploop = root->children;
  menu_item table[26];
  int i;
  for(i = 0; i < 26; i++) table[i] = NULL;
  /*first attempt to assign keys based on capitalized letters*/

  while( sploop != NULL )
    {
    for(i = 0; i < strlen(sploop->name); i++)
      {
      if(sploop->name[i] >= 'A' && sploop->name[i] <= 'Z')
        { 
        if(table[sploop->name[i]-'A'] == NULL) 
          {
          sploop->hotkey = sploop->name[i]-'A'+'a'; 
          table[sploop->name[i]-'A'] = sploop;
          break;
          }
        } 
      }
    sploop = sploop->sibling;
    } 
    /*then assign any leftovers lower case letters from their names*/ 
  sploop = root->children;
  while( sploop != NULL )
    {
    for(i = 0; i < strlen(sploop->name); i++)
      {
      if(sploop->name[i] >= 'a' && sploop->name[i] <= 'z')
        { 
        if(table[sploop->name[i]-'a'] == NULL && sploop->hotkey == '\0') 
          {
          sploop->hotkey = sploop->name[i]; 
          table[sploop->name[i]-'a'] = sploop;
          break;
          }
        }
      }
    sploop = sploop->sibling;
    } 
    /*TODO what if I all the letters in the word are taken*/
    /*TODO what if there are no letters left*/
  sploop = root->children;
  while(sploop != NULL) 
    {
    assign_hotkeys(sploop);
    sploop = sploop->sibling;
    }
  }

menu_item initialize_menu(char *menu_path)
  {
  menu_item current_item;
  char *string;
  enum read_state state;
  int item_index, i;
  enum attribute attr = NONE;
  FILE *fp = fopen(menu_path, "r");
  char *exit_name = (char *)malloc(sizeof(char) * 5 );
  menu_item root_menu = new_menu_item("", '\0', "");
  menu_item exit_item = new_menu_item(exit_name, 'x', "");
  strcpy(exit_name, "eXit"); 
  append_menu_item(root_menu, exit_item);
  item_index = -1; //TODO handle the error where we try to add data without a HEAD block first  
  process_one_line( fp, &string, &state ); 
  while( state != END ) 
    { 
    switch(state)
      {
      case HEAD : 
       	attr = NONE;  
        item_index = item_index+1; 
        current_item = locate(root_menu, string);
        break;
      case BODY : 
        switch(attr)
          {
          case NONE : body_append( &(current_item->command), string); break;
          case SHUTDOWN : body_append( &(current_item->shutdown), string); break;
          defaut : free(string); 
          } 
        break;
      case ATTR : 
        if(strcmp(string, "SHUTDOWN") == 0) attr = SHUTDOWN;
        free(string);
      }
    process_one_line( fp, &string, &state ); 
    }
  assign_hotkeys(root_menu);
  menu_normalize_names(root_menu);
  return root_menu;
  }
