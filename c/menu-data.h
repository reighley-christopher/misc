typedef struct _menu_item
  {
  char *name;
  char hotkey;
  struct _menu_item *parent;
  struct _menu_item *children;
  struct _menu_item *sibling; 
  char *command;
  char *shutdown;
  } *menu_item;

menu_item new_menu_item( char *name, char hotkey, char *command );

void append_menu_item( menu_item parent, menu_item child );

menu_item initialize_menu(char *menu_path);
