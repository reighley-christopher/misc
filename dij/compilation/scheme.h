
void xscheme_init();

void xscheme_eval( char *index );

void xscheme_consume(char c);

char *xscheme_push();

void xscheme_file( char *filename );

void xscheme_node( char *name, int count );

void xscheme_leaf( char *name, char *parameter );

void xscheme_list( char *name );

void xscheme_ground( char *name );

void xscheme_complete();

