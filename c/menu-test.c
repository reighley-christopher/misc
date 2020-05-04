#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu-data.h"

menu_item root_menu;

menu_item xinitialize_menu(char *menu_path);

void print_menu( menu_item menu )
  {
  menu_item sploop = menu->children;
  printf("NAME: %s HOTKEY: %c\nCOMMAND:\n%s\nSHUTDOWN:\n%s\nCHILDREN:\n", menu->name, menu->hotkey, menu->command, menu->shutdown);
  while(sploop) 
    {
    printf("%s\n", sploop->name);
    sploop = sploop->sibling;
    } 
  printf("\n");
  sploop = menu->children;
  while(sploop)
    {
    print_menu(sploop);
    sploop = sploop->sibling;
    }
  }

int main(int argc, char *argv[])
  {
  menu_item root_menu = xinitialize_menu("menu.menu");
  print_menu(root_menu);
  return 0;
  } 
