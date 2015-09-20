
struct _object_type
   {
   char placeholder;
   };

extern struct _object_type
   TYPE_SCALAR,
   TYPE_HOT_CALL,
   TYPE_LVALUE,
   TYPE_F_NODE,
   TYPE_SYMBOL,
   TYPE_EMPTY;

struct _object
   {
   struct _object_type *type;
   long int value;
   };

/*a linked list of parameters to a function (some of which may be blank)*/
struct _parameter
   {
   char               curry_flag;
   struct _object     value;
   struct _parameter *next; 
   };


extern long int globals[];
extern long int max_global;

struct _stack_member
   {
   struct _object value;
   struct _stack_member *under;
   };


/*
void *c_box( void *f, struct iFGraph *fgraph );
struct iCode *dij_box( int *code );
*/

int array_contains( int *array, int size, int element );

int *array_add( int *array, int size, int element );

int *array_remove( int *array, int size, int element );

int *array_copy( int *array, int size );

int get_name( char *name );

char *decode_name( int code );

