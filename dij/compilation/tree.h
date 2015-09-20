typedef long int DIJ_WORD;
typedef void (*command)( DIJ_WORD arg );

struct _tree
{
   command f;
   int arg;
   struct _tree *left;
   struct _tree *right;
};

struct _tree *create_tree( command f, int arg, struct _tree *left, struct _tree *right);

void execute_tree( struct _tree *t );
