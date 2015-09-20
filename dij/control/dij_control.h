/*prototypes and forshadowing of things to come*/
struct iCode;
struct iFGraph;
struct iProcess;
struct iContext;

/*interfaces for the control of the running dij system*/

/*an iChannel is a simple abstraction for both blocking io, and context switches
  the send method is obvious.  The recieve method requires an iChannelClient
  which may be asked to wait for a message, rather than be provided one immediately
  If the channel is closed before data is ready, then any ChannelClient waiting for
  data will be asked to terminate (or at least stop waiting).
  abandon is the opposite of receive, the ChannelClient has given up waiting for its
  data.
*/

struct iChannelClient
   {
   void *CC;
   void (*sleep)(struct iChannelClient *self);
   /*SPEC wake must be able to cope with a situation in which *data is null*/
   /*SPEC wake might be called before receive completes*/
   void (*wake)(struct iChannelClient *self, struct _stack_member *data);
   void (*terminate)(struct iChannelClient *self);
   };

struct iChannel
   {
   void *C;
   /*SPEC send must able to cope with a situation in which *data is null*/
   void (*send)(struct iChannel *self, struct _stack_member *data);
   void (*receive)( struct iChannel *self, struct iChannelClient *client);
   void (*abandon)( struct iChannel *self, struct iChannelClient *client);
   void (*close)( struct iChannel *self );
   };

/*this is a persistant object representing the different bodies of code in the system
  the method new initializes a new iMachine to execute a given block of code.
  The parameter context tells the new method how the variables will be bound to the
  memory.
*/
struct iCode
   {
   void *C;
   struct iMachine *(*new)
      ( 
      struct iCode *self,
      struct iContext *context,
      struct iProcess *process,
      struct iFGraph *fgraph
      );
   };

struct iChannel *channel_new();

struct iNamespace
   {
   void *N;
   int (*variable_offset)
      (
      struct iNamespace *self,
      int code
      );
   int (*what_name)(struct iNamespace *self, int offset);
   void (*size_namespace)(struct iNamespace *self, int num_parameters, int num_locals, int num_returns, int num_anonymous); 
   void (*write_namespace)(struct iNamespace *self, int *parameters, int *locals, int *returns);
   int (*get_size)(struct iNamespace *self);
   void (*get_sizes)(struct iNamespace *self, int *num_parameters, int *num_locals, int *num_returns, int *num_anonymous);
   void (*merge)(struct iNamespace *self, struct iNamespace *first, struct iNamespace *second);
   void (*open)(struct iNamespace *self, struct iNamespace *source);
   void (*close)(struct iNamespace *self);
   int (*bind)(struct iNamespace *self);
   void (*curry)(struct iNamespace *self);
   };

struct iContext
   {
   void *C;
   struct iNamespace *i_namespace;
   void (*append_codeblock)(struct iContext *self, struct iCode *block);
   void (*merge)(struct iContext *self, struct iContext *first, struct iContext *second);
   struct iCode *(*iterate)(struct iContext *self, void **token);
   };


struct iMachine
   {
   void *M; /*there is some unstructured data specific to the machine*/
   /*the machine must be cleaned up when it is finished*/
   void (*destroy)(struct iMachine *self); 
   /*this runs the machine, modifying the common memory as necessary, it will
     return 0 if the machine has completed normally and should be cleaned up, 
     -1 if the machine has paused to wait on another process, otherwise a pointer 
     to an exception structure giving details about the why machine stopped*/
   struct iException *(*run)(struct iMachine *self);
   /*a machine may be sent a message, just like a channel*/
   /*void (*send)(struct _stack_member *data);*/
   /*the machine should probably be creating its own channels*/
   };

/*iProcess describes a single executing environment including a list of executable blocks
  to be executed in order and their shared memory space.  The original size of the
  memory is set by initialize.  The executable code, implementing the iMachine interface,
  is added to by the append method.  The attach method puts the process onto the list
  of executing processes (to start it), the detach method removes it (to stop it).  The
  get_memory method provides pointers to the memory allocated to the process*/

struct iProcess
   {
   void *P;
   void (*initialize)
      (
      struct iProcess *self, 
      int memory_size, 
      int return_offset, 
      int return_size,
      struct iChannel *exit_channel
      );
   void (*append)(struct iProcess *self, struct iMachine *machine);
   void (*attach)(struct iProcess *self);
   void (*detach)(struct iProcess *self);
   void (*get_memory)
      (
      struct iProcess *self, 
      long int **memory, 
      struct _object_type ***types, 
      struct iChannel **exit_channel
      );
   };

/*this is a stub, for error handling*/
struct iException
   {
   void *E;
   char *(*message)();
   };

/*iFGraph is the main interface to the control subsystem.  The execution subsystem will
  have called fgraph_new and be using it essentially to translate iCode interfaces
  (which represent code that may execute) into iProcess interfaces (which represent
  code actually executing)*/
struct iFGraph
   {
   void *FG;
   /*it is possible that load should absorb all the functionality of invoke in dij_machine.c*/
   struct iProcess *(*load)
      ( 
      struct iFGraph *self, 
      void *fnode, 
      struct iChannel *exit_channel,
      int left 
      );
   void *(*ground)
      ( 
      struct iFGraph *self, 
      struct iContext *context
      );
   void *(*apply)( struct iFGraph *self, void *fnode, struct _parameter *p );
   void *(*fcurry)( struct iFGraph *self, struct _parameter *p );
   void *(*spawn)( struct iFGraph *self, void *fnode );
   void *(*ljoin)( struct iFGraph *self, void *fnode_left, void *fnode_right );
   void *(*njoin)( struct iFGraph *self, void *fnode_left, void *fnode_right );
   int  (*is_loadable)( struct iFGraph *self, void *fnode, int left );
   void (*reference)(struct iFGraph *self, void *fnode);
   void (*unreference)(struct iFGraph *self, void *fnode);
   void (*destroy)( struct iFGraph *self);
   void (*debug)( struct iFGraph *self, int n );
   };


struct iFGraph *fgraph_new();
struct iProcess *new_iProcess();
struct iContext *new_context();
struct iNamespace *new_namespace();

void fgraph_debug(struct iFGraph *self, void *fnode, int operation );
void process_go();
