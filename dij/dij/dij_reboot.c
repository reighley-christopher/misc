struct _process
   {
   int *memory;
   int *types;
   struct iMachine *head, *tail;
   struct _process *next, *last;
   struct iChannel *exit_channel;
   };

struct _process *first_process, *last_process;

struct iMachine
   {
   struct iMachine *next;
   struct _process *parent;
   void *M;
   void (*load)(struct iMachine *self, int *memory, int *types);
   void (*destroy)(struct iMachine *self); 
   void (*run)(struct iMachine *self);
   };

struct function_node 
{
   struct function_node *left;
   struct function_node *right;
   void (*process)();
   union {
         struct {
                } primitive;
         struct {
                } 
         } data;   
}

struct _process *traverse_function_graph( struct function_node *node )
{
   struct iMachine *left, *right;
   left = traverse_function_graph
}
