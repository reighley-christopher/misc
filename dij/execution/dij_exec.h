struct _dij_machine
   {
   char mode;
   struct _stack_member *stack;
   int *code;
   int inst_pointer;
   int anonymous_in_ptr;
   int *variables;
   struct _object_type **types;
   struct iChannel *anonymous_out;
   struct _fcontext *context;
   struct iProcess *process;
   struct iFGraph *fgraph;
   /*the machine has a dedicated channel interface
     so that it can recieve a return value after it has called
     a subroutine, probably only send will be implemented*/
   struct iChannel wait_channel;
   struct iException error_condition;
   };

void inst_push( struct _dij_machine *M );

void inst_apply( struct _dij_machine *M );

void inst_curry( struct _dij_machine *M );

void inst_halt( struct _dij_machine *M );

void inst_check( struct _dij_machine *M );

void inst_reference_variable( struct _dij_machine *M );

void inst_reference_global( struct _dij_machine *M );

void inst_lvalue_name( struct _dij_machine *M );

void inst_assign( struct _dij_machine *M );

void inst_add( struct _dij_machine *M );

void inst_lvalue_call( struct _dij_machine *M );

void inst_anonymous_parameter( struct _dij_machine *M );

void inst_anonymous_return( struct _dij_machine *M );

void inst_spawn( struct _dij_machine *M );

void inst_namespace_join( struct _dij_machine *M);

void inst_left_join( struct _dij_machine *M );

void inst_noop( struct _dij_machine *M );

void inst_do_mark( struct _dij_machine *M );

void inst_if_mark( struct _dij_machine *M );

void inst_test( struct _dij_machine *M );

void inst_not( struct _dij_machine *M );

void inst_neg( struct _dij_machine *M );

void inst_equal( struct _dij_machine *M );

void dij_write( int word );

void dij_push_data();

void dij_pop_data();

void dij_push_code();

void dij_pop_code();

void dij_define_global();

void dij_note_number( void (*note_me)( int later ) );

void dij_record_number( int later );

void dij_note_lvalue( int later );

void dij_note_rvalue( int later );

void dij_push_if();

void dij_push_do();

void dij_control_test();

void dij_control_marker();

void dij_pop_control();

void debug( struct _dij_machine *M );

void *c_box( void *f, struct iFGraph *fgraph);

struct iCode *dij_box( int *code );

struct iFGraph *fgraph;

struct _object object( int value, struct _object_type *type );
