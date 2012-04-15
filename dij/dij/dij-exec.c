#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>

#define TYPE_SCALAR 0
#define TYPE_HOT_CALL 1
#define TYPE_LVALUE 2
#define TYPE_F_NODE 3

#define MODE_UNINITIALIZED 0
#define MODE_RUN 1
#define MODE_HALT 2
#define MODE_WAIT 3

struct _object
   {
   char type;
   int value;
   };

struct _stack_member
   {
   struct _object value;
   struct _stack_member *under;
   };



/*
this doesn't seem necessary anymore because the relevant detail is contained in partial_call now.
struct _function_pair
{
   struct _function_metadata *right;
   struct _function_metadata *left;
   };*/

struct _lvalue
{
   int parameter;
  void (*liquidate)(int parameter, struct _parameter *rvalues, struct _machine *M );
};

void graft( struct _stack_member **stack, struct _stack_member *addme );

struct _machine *first_machine;
struct _machine *last_machine;


struct _object object( int value, char type ) 
{ struct _object ret; ret.value = value; ret.type = type; return ret; } 

void push( struct _stack_member **stack, struct _object value )
   {
   struct _stack_member *new_member = (struct _stack_member *)malloc( sizeof(struct _stack_member) );
   new_member->value = value;
   new_member->under = *stack;
   *stack = new_member;
   }

struct _object pop( struct _stack_member **stack )
   {
   struct _stack_member *killme = *stack;
   struct _object ret = killme->value;
   *stack = killme->under;
   free( killme );
   return ret;
   }

void graft( struct _stack_member **stack, struct _stack_member *addme )
   {
   struct _stack_member *last = addme;
   if( !last ) { return; }
   while( last->under ) { last = last->under; }
   last->under = *stack;
   *stack = addme; 
   }

/*this constructor is no longer very good
struct _execution_pair *execution_pair( half_execution_mechanism left, half_execution_mechanism right )
{
  struct _execution_pair *ret = malloc(sizeof(struct _execution_pair));
  ret->left = left;
  ret->right = right;
  return ret;
  }*/

void evaluate_partial_call( struct _partial_call *pc, struct _machine *M );
typedef void (*instruction)( struct _machine *M);

void partial_call_execution_mechanism_right( struct _function_metadata *meta, struct _parameter *p, struct _machine *M0 )
{
   /*the first parameter is going to be a partial_call structure, the
   remaining parameters will be used to populate the new partial call*/
   struct _partial_call *pc = (struct _partial_call *)(meta->code);
   struct _parameter *param = p;
   struct _partial_call *new_pc = copy_partial_call( pc );
   struct _parameter *c = new_pc->first_param;
   struct _stack_member *ret;
   printf("start evaluation\n");
   while( c && !c->curry_flag ) { c = c->next; } 
   while( c && param )
   {
      c->value = param->value;
      c->curry_flag = 0;
      param = param->next;
      while( c && !c->curry_flag ) { c = c->next; }
   }
   while( param ) 
   {
      if( param->curry_flag ) { curry( new_pc ); }
      else                    { apply( new_pc, param->value ); } 
      param = param->next;
   }
   printf("ready to evaluate\n");
   evaluate_partial_call( new_pc, M0 );     
}

void lvalue_call_liquidate( int address, struct _parameter *p, struct _machine *M );

void partial_call_execution_mechanism_left( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
  struct _partial_call *pc = (struct _partial_call *)(meta->code);
  printf("partial_call_execution_mechanism_left %x\n", pc);
  lvalue_call_liquidate( (int)pc, p, M ); 
}


void channel_exit_procedure( struct _machine *M, void *return_channel )
{
   int i;
   struct _stack_member *ret = 0;
   for(i = M->context->num_returns-1; i >= 0; i-- )
      {
      push( &(ret), object( M->variables[i+M->context->num_parameters+M->context->num_locals], TYPE_SCALAR ) );
      }
   
   channel_send( (struct _channel *)return_channel, ret );
   printf("complete halt\n");
}

void namespace_exit_procedure( struct _machine *M, void *c )
{
   /*rather than returning the variables along a designated channel, the machine is given a new code body, the instruction pointer is reset
     and the machine returns to run mode*/
   struct _namespace_chain *chain = (struct _namespace_chain *)c;
   if( chain->sploop->next ) 
      {
      chain->sploop = chain->sploop->next;
      M->code = chain->sploop->instructions;
      M->inst_pointer = 0;
      M->mode = MODE_RUN;
      printf("namespace continuing\n");
      } else {
      channel_exit_procedure( M, chain->exit_channel );
      free( chain );
      }  
}

/*
All this stuff can be removed since reforming the _function_metadata structure.
struct _joined_pair
{
  struct _partial_call *left;
  struct _partial_call *right;
};

void joint_execution_mechanism_left( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
  *//*the joint execution mechanism is used when the partial call is the product of a left join operation.
    the first parameter will direct the machine to the pair of partial calls from which this is constructed.
    the remaining parameters will be passed to it.
    *//*
  struct _partial_call *pc = copy_partial_call(((struct _joined_pair *)(p->value.value))->left);
  lvalue_call_liquidate( (int)pc, p->next );   
}

void joint_execution_mechanism_right( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
  struct _partial_call *pc = copy_partial_call(((struct _joined_pair *)(p->value.value))->right);
     *//*all this was copy-pasted from partial_call_execution_mechanism, so there is obviously some
       redundancy in this structure that needs to be worked out.*//*
   struct _parameter *param = p->next;
   *//*struct _partial_call *new_pc = copy_partial_call( pc );*//*
   struct _parameter *c = pc->first_param;
   struct _stack_member *ret;
   while( c && !c->curry_flag ) { c = c->next; } 
   while( c && param )
   {
      c->value = param->value;
      c->curry_flag = 0;
      param = param->next;
      while( c && !c->curry_flag ) { c = c->next; }
   }
   while( param ) 
   {
      if( param->curry_flag ) { curry( pc ); }
      else                    { apply( pc, param->value ); } 
      param = param->next;
   }
   evaluate_partial_call( pc, M );   
   
} */

/*mostly the same as dij_execution_mechanism, except for the different exit procedure*/
void namespace_execution_mechanism( struct _function_metadata *f, struct _parameter *p, struct _machine *M0 )
{
   struct _machine *M = new_machine();
   struct _parameter *sploop = p;
   struct _stack_member *ret = 0;
   struct _channel *return_channel = malloc(sizeof (struct _channel) );
   struct _namespace_chain *chain = malloc(sizeof (struct _namespace_chain));
   int i = 0;
   return_channel->first_client = 0;
   return_channel->last_client = 0;
   return_channel->first_message = 0;
   return_channel->last_message = 0;
   chain->exit_channel = return_channel;
   chain->sploop = (struct _function_codeblock *)f->code;
   printf("I am ready\n");
   channel_receive( return_channel, M0 );

   M->variables = malloc( sizeof(int) * (f->num_parameters + f->num_locals + f->num_returns) );
   printf("loading variables (namespace join)\n");
   for( i = 0; i <  f->num_parameters ; i++ )
      {
      M->variables[i] = sploop->value.value;
      sploop = sploop->next;
      printf("%x.\n", M->variables[i]);
      }
   printf("done\n");
   M->mode = MODE_RUN;
   M->context = f;
   M->code = ((struct _function_codeblock *)f->code)->instructions; 
   M->inst_pointer = 0;
   M->next_anonymous_parameter = sploop;
   M->exit_procedure = namespace_exit_procedure;
   M->exit_datum = chain;
   if(M0) M0->mode = MODE_WAIT;
   /*
   run_machine(M);
   */
}

/*I'm not sure I need these anymore, now that I am passing the metadata to the execution mechanism
void dij_execution_mechanism_right( struct _function_metadata *meta, struct _parameter *p, struct _machine *M ) {    
   struct _function_metadata *f = ((struct _function_pair *)(p->value.value))->right;
   dij_execution_mechanism( f, p, M );
   }

void dij_execution_mechanism_left( struct _function_metadata *meta, struct _parameter *p, struct _machine *M ) {  
   struct _function_metadata *f = ((struct _function_pair *)(p->value.value))->left;
   dij_execution_mechanism( f, p, M);
   }
*/

int dij_c_call(void *f, int argc, int *argv );

void C_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   int size = 0;
   void *f;
   int *v;
   int i;
   struct _stack_member *ret = malloc(sizeof (struct _stack_member) );
   struct _parameter *sploop = p;
   f = meta->code;
   while( !sploop->curry_flag )
   {
     size = size+sizeof(int);
     sploop = sploop->next;
   }
   sploop = p;
   v = (int *)malloc(size);
   i = 0;
   while( !sploop->curry_flag )
   {
     v[i] = sploop->value.value; /*printf("!!!%s\n", sploop->value.value );*/
     sploop = sploop->next;
     i = i+1;
   }
   printf("calling : %d\n", size );
   ret->value.value = dij_c_call(f, size, v);
   ret->value.type = TYPE_SCALAR;
   ret->under = 0;
   free(v);
   if(M) graft( &(M->stack) ,ret);
}

struct _bidirectional_bridge
{
   struct _channel upstream;
   struct _channel downstream;
};

void send_upstream_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   /*the first parameter is a _bidirectional_bridge, the remaining parameters are formed
     into a message and sent onto the upstream channel, whereupon the machine is suspended
   */
   struct _stack_member *ret = 0;
   struct _bidirectional_bridge *bridge = (struct _bidirectional_bridge *)meta->code;
   struct _parameter *sploop = p;
   printf("send upstream execution mechanism\n");
   while( sploop && !(sploop->curry_flag) )
      {
      push( &ret, sploop->value );
      sploop = sploop->next;
      }
   channel_send( &(bridge->upstream), ret );
   printf("back out at execution\n");
   if(M) M->mode = MODE_WAIT;  
}

void receive_upstream_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   /*the only parameter is a bidirectional bridge, the machine is set to recieve from the upstream end*/
  struct _bidirectional_bridge *bridge = (struct _bidirectional_bridge *)meta->code;
   printf("recieve upstream mechanism\n");
   channel_receive( &(bridge->upstream), M );   
}

void send_downstream_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   /*the first parameter is a _bidirectional_bridge, the remaining parameters are formed
     into a message and sent onto the downstream channel, whereupon the machine is suspended
   */
   struct _stack_member *ret = 0;
   struct _bidirectional_bridge *bridge = (struct _bidirectional_bridge *)meta->code;
   struct _parameter *sploop = p;
   printf("send downstream mechansim\n");
   while( sploop && !(sploop->curry_flag) )
      {
      printf("sending ... %x\n", sploop->value );
      push( &ret, sploop->value );
      sploop = sploop->next;
      }
   channel_send( &(bridge->downstream), ret );
   printf("back out at execution %x\n", M);
   if(M) M->mode = MODE_WAIT;  
   printf("mode switch okay\n");
}

void receive_downstream_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   /*the only parameter is a bidirectional bridge, the machine is set to recieve from the downstream end*/
  struct _bidirectional_bridge *bridge = (struct _bidirectional_bridge *)meta->code;
   printf("recieve downstream mechanism\n");
   channel_receive( &(bridge->downstream), M );   
}



/*initialization code*/

struct _buffer_chunk
{
  int data[1024];
  int size;
  struct _buffer_chunk *next;
};

struct _buffer
{
   struct _buffer *up;
   struct _buffer_chunk first;
   struct _buffer_chunk *last;
};

struct _buffer *write_buffer = 0;
struct _function_metadata *write_code = 0;

void open_buffer()
{
  struct _buffer *new_buffer;
  new_buffer = malloc(sizeof( struct _buffer ) );
  new_buffer->up = write_buffer;
  write_buffer = new_buffer;
  new_buffer->last = &(new_buffer->first);
  new_buffer->first.size = 0;
  new_buffer->first.next = 0;
}

int *close_buffer()
{
  int *ret;
  int size = 0;
  int w_index = 0;
  int r_index = 0;
  struct _buffer_chunk *sploop = &(write_buffer->first);
  struct _buffer_chunk *kill_me;
  struct _buffer *kill_buffer;
  while( sploop )
  {
    size = size + sploop->size;
    sploop = sploop->next;
  }
  ret = malloc( sizeof(int) * size );
  sploop = &(write_buffer->first);
  while( sploop )
  {
    for( r_index = 0; r_index < sploop->size; r_index++, w_index++ )
    {
       ret[w_index] = sploop->data[r_index];
    }
    sploop = sploop->next;
  }
  sploop = write_buffer->first.next;
  while(sploop) { kill_me = sploop; sploop = sploop->next; free(kill_me); }
  kill_buffer = write_buffer;
  write_buffer = write_buffer->up;
  free(kill_buffer);
  return ret;
}

void dij_write( int word )
{
   if( write_buffer == 0 ) { open_buffer(); }
   write_buffer->last->data[write_buffer->last->size] = word;
   write_buffer->last->size++;
   if( write_buffer->last->size == 1024 )
   {
      write_buffer->last->next = malloc(sizeof(struct _buffer_chunk));
      write_buffer->last = write_buffer->last->next;
      write_buffer->last->size = 0;
      write_buffer->last->next = 0;
   }
}

/*this procedure will fail if we are at a 1024 word boundry, but that is okay 
  because it will never be used at 0, and it will always be used in the global
  space, which shouldn't get larger than about 3 words long*/
int dij_unwrite()
{
  int ret;
  write_buffer->last->size--;
  ret = write_buffer->last->data[ write_buffer->last->size ];
  return ret;
}

void dij_push_data()
{
   open_buffer();
}

void dij_pop_data()
{
   int *result = close_buffer();
   dij_write( (int)result );
}

void dij_push_code()
{
   struct _function_metadata *f;
   open_buffer();
   f = new_function_metadata( dij_execution_mechanism, 0 );
   f->up = write_code;
   write_code = f;
}

void dij_pop_code()
{
   struct _function_metadata *left = copy_function_metadata(write_code);
   struct _partial_call *pc; 
   write_code->exec = dij_execution_mechanism;
   left->exec = dij_execution_mechanism;
   pc = initialize_partial_call( left, write_code, 0 );
   write_code->code = new_function_codeblock( close_buffer() );
   left->code = write_code->code;
   printf("code::%x  partial call:: %x\n", write_code->code, pc );
   write_code = write_code->up;
   dij_write( (int) pc );
}

void dij_define_global()
{
   int number;
   int value;
   printf("define global\n");
   number = dij_unwrite();
   value = dij_unwrite();
   globals[number] = value;
   if( number > max_global ) max_global = number;
}

/*utility functions for editing arrays of int*/

int array_contains( int *array, int size, int element )
{
   int i;
   for(i = 0; i < size; i++ )
   {
      if( array[i] == element ) return -1;
   }
   return 0;
}

int *array_add( int *array, int size, int element )
{
   int *ret = malloc(sizeof(int) * (size + 1 ) );
   int i;
   for(i = 0; i < size; i++ )
   {
      ret[i] = array[i];
   }
   ret[i] = element;
   if( array ) free(array);
   return ret;
}

int *array_remove( int *array, int size, int element )
{
   int *ret = malloc(sizeof(int) * (size - 1 ) );
   int i;
   int j = 0;
   for(i = 0; i < size; i++ )
   {
      if( array[i] != element )
      {
          ret[j] = array[i]; j++;         
      }
   }
   free(array);
   return ret;
}

int *array_copy( int *array, int size )
{
   int *ret = malloc(sizeof(int) * size );
   int i;
   for( i = 0; i < size; i++ ) { ret[i] = array[i]; }
   return ret;
}

/*end utility functions*/

/*this is a silly way of doing this but I like it much better somehow*/

void dij_note_nothing( int later )
{
}

void dij_note_lvalue( int later )
{
   /*variable number later is being used as an lvalue in this context, 
     is this the first use? ie is this possibly a return value? if so make sure it has a slot.*/
   if( ! ( 
         array_contains(write_code->parameters, write_code->num_parameters, later ) || 
         array_contains(write_code->locals, write_code->num_locals, later) || 
         array_contains(write_code->returns, write_code->num_returns, later) ) )
        { 
        write_code->returns = array_add(write_code->returns, write_code->num_returns, later);
        write_code->num_returns++;  
        }

}

void dij_note_rvalue( int later )
{
   /*variable number later is being used as an rvalue in this context,
     is this the first use? ie is this a parameter? if so make sure it has a slot. 
     otherwise, we now know that it is not a return value, so move it out of that group.*/
    if( ! (
          array_contains(write_code->parameters, write_code->num_parameters, later ) || 
          array_contains(write_code->locals, write_code->num_locals, later ) ) )
    {
       if( array_contains(write_code->returns, write_code->num_returns, later ) )
       {
          write_code->returns = array_remove(write_code->returns, write_code->num_returns, later);
          write_code->locals = array_add(write_code->locals, write_code->num_locals, later);
          write_code->num_returns--;
          write_code->num_locals++;
       } else {
          write_code->parameters = array_add(write_code->parameters, write_code->num_parameters, later);
          write_code->num_parameters++;
       }
    }
}

void (*number_notes)( int later ) = dij_note_nothing;

void dij_note_number( void (*note_me)( int later ) )
{
   number_notes = note_me; 
}

void dij_record_number( int later )
{
   number_notes(later);
   number_notes = dij_note_nothing;
}

/*end silly thing*/

/*control structures*/
struct _control_structure
{
  int control_type; /*-1 == if ; 0 == do */
  int *control_edit;
  int *marker_edit;
  struct _control_structure *next;
};

struct _control_structure *current_control_structure = 0;

void push_control_structure()
{
  struct _control_structure *ret = malloc(sizeof(struct _control_structure));
  ret->next = current_control_structure;
  current_control_structure = ret;
}

void pop_control_structure()
{
  struct _control_structure *kill_me = current_control_structure;
  current_control_structure = current_control_structure->next;
  free(kill_me);
}

void dij_push_do()
{
  push_control_structure();
  current_control_structure->control_type = 0;
  dij_write((int)inst_noop);
  current_control_structure->control_edit = write_buffer->last->data + write_buffer->last->size;
  dij_write(0);
}

void dij_push_if()
{
  push_control_structure();
  current_control_structure->control_type = -1;
  dij_write((int)inst_noop);
  current_control_structure->control_edit = write_buffer->last->data + write_buffer->last->size;
  dij_write(0);
}

int final_distance(int *target)
{   
   struct _buffer_chunk *sploop;
   int chunks;
   int offset;
   sploop = &(write_buffer->first);
   while( sploop )
      {
      if( target >= sploop->data && target < sploop->data + 1024 )
         {
         offset = target - sploop->data;
         chunks = 0;
         }
      chunks = chunks+1;
      sploop = sploop->next;
      }
   return (chunks-1)*1024 + write_buffer->last->size - offset;
}

void dij_pop_control()
{
*(current_control_structure->control_edit) = final_distance(current_control_structure->control_edit);
}

void dij_control_test()
{
  dij_write((int)inst_test);
  current_control_structure->marker_edit = write_buffer->last->data + write_buffer->last->size;
  dij_write(0);
}

void dij_control_marker()
{
  if(current_control_structure->control_type)
    {
    dij_write((int)inst_if_mark);
    } else {
    dij_write((int)inst_do_mark);
    }
    *(current_control_structure->marker_edit) = final_distance(current_control_structure->marker_edit);
}

/*end control structures*/


struct _partial_call *c_box( int func_ptr )
{
  struct _function_metadata *m = 
    new_function_metadata( C_execution_mechanism, (void *)func_ptr );
  return initialize_partial_call( m, m, 0 );
}

struct _partial_call *dynamic_load( char *library, char *symbol )
{
   printf( "!!!!!!!!!!load: %s, %s\n", library, symbol );
   return c_box( (int)dlsym( dlopen(library, 1), symbol ) );
}

/*end initialization code*/

void initialize()
{
  globals[0] = (int)c_box( (int)dynamic_load );
}

void go()
{
  struct _machine *current;
  struct _machine *sploop;
  printf("ready\n");
  curry( (struct _partial_call *)globals[1] );
  evaluate_partial_call( (struct _partial_call *)globals[1], 0  );
  printf("ready to begin\n");
  while( first_machine )
     {
     current = first_machine;
     first_machine = first_machine->next;
     if( last_machine = current ) last_machine = first_machine;
     run_machine( current );
     printf("context switch\n");
     sploop = first_machine;
     while( sploop )
        {
        printf("\tmachine_list: %x %d\n", sploop, sploop->mode);
        sploop = sploop->next;
        }
     /*if(current->mode == MODE_HALT ) destroy_machine(current);*/
     }
  printf("machine has halted normally\n");
}

/*debugging functions*/

void print_stack( struct _machine *M )
   {
   struct _stack_member *sploop;
   sploop = M->stack;
   printf("*PRINT_STACK*\n");
   while( sploop )
      {
      printf("\tprint_stack: %x\n", sploop->value.value);
      sploop = sploop->under;
      }
   }

void machine_list( struct _machine *M )
   {
   struct _machine *sploop;
   sploop = first_machine;
   while( sploop )
      {
      printf("\tmachine_list: %x %d\n", sploop, sploop->mode);
      sploop = sploop->next;
      }
   }

void constants( struct _machine *M )
   {
   int i;
   for(i = 0; i <= max_global; i++ )
      {
      printf("\tconstants: %x\n", globals[i]);
      }
   }

void variables( struct _machine *M )
   {
   int i;
   int j = 0;
   struct _function_metadata *md = M->context;
   printf("\tvariables parameters\n");
   for(i = 0; i < md->num_parameters; i++ )
      {
      printf("\tvariables %d:%x\n", md->parameters[i], M->variables[j]);
      j++;
      }
   printf("\tvariables locals\n");
   for(i = 0; i < md->num_locals; i++ )
      {
      printf("\tvariables %d:%x\n", md->locals[i], M->variables[j]);
      j++;
      }
   printf("\tvariables returns\n");
   for(i = 0; i < md->num_returns; i++ )
      {
      printf("\tvariables %d:%x\n", md->returns[i], M->variables[j]);
      j++;
      }
   }

void partial_call( struct _machine *M )
   {
     /*the object on the top of the stack is a _partial_call, which I will inspect*/
     struct _partial_call *pc = (struct _partial_call *)M->stack->value.value;
     struct _parameter *sploop = pc->first_param;
     while(sploop)
       {
	 if(sploop->curry_flag)
	   {
	   printf("\tpartial_call: []\n");  
	   } else {
	   printf("\tpartial_call: %x\n", sploop->value.value );
	   }
	 sploop = sploop->next;
       }
   }

instruction debugs[] =
   {
   print_stack,
   machine_list,
   constants,
   variables,
   partial_call
   };

void debug( struct _machine *M )
   {
   int ref;
   instruction exec;
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   exec = debugs[ref];
   exec(M);
   }
/*end debugging functions*/


void yylex();

int main( int argc, char *argv[])
{
   yylex();
   initialize();
   go();
}
