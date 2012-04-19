/*TODO eliminate references to standard libraries*/
#include <stdlib.h>
#include <stdio.h>

#include "dij_misc.h"
#include "../control/dij_control.h"
#include "dij_exec.h"

#define MODE_UNINITIALIZED 0
#define MODE_RUN 1
#define MODE_HALT 2
#define MODE_WAIT 3
#define MODE_FAULT 4

struct _object_type 
   TYPE_SCALAR = {0},
   TYPE_HOT_CALL= {1},
   TYPE_LVALUE = {2},
   TYPE_F_NODE = {3},
   TYPE_SYMBOL ={4},
   TYPE_EMPTY = {5};

typedef void (*instruction)( struct _dij_machine *M );

struct _lvalue
  {
  void *parameter;
  void (*liquidate)(void *parameter, struct _parameter *rvalues, struct _dij_machine *M );
  };

void graft( struct _stack_member **stack, struct _stack_member *addme );

struct _object object( int value, struct _object_type *type ) 
{ struct _object ret; ret.value = value; ret.type = type; return ret; } 

void push( struct _stack_member **stack, struct _object value )
   {
   struct _stack_member *new_member = 
      (struct _stack_member *)malloc( sizeof(struct _stack_member) );
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

/*
this looks like old code, its function should now be performed by the new method on
the dij iCode implementation
*/
/*
struct iException *dij_execution_mechanism
   (
   struct _fcontext *f, 
   struct _parameter *p, 
   struct _dij_machine *M0
   )
   {
   struct _dij_machine *M = (struct _dij_machine *)malloc(sizeof(struct _dij_machine));
   struct _parameter *sploop = p;
   struct _stack_member *ret = 0;
   struct _channel *return_channel = malloc(sizeof (struct _channel) );
   int i = 0;
   return_channel->first_client = 0;
   return_channel->last_client = 0;
   return_channel->first_message = 0;
   return_channel->last_message = 0;
   printf("I am ready\n");
   channel_receive( return_channel, M0 );

   M->variables = malloc( sizeof(int) * (f->num_parameters + f->num_locals + f->num_returns) );
   printf("loading variables\n");
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
   M->exit_procedure = channel_exit_procedure;
   M->exit_datum = return_channel;
   if(M0) M0->mode = MODE_WAIT;
   run_machine(M);
   }
*/

void invoke
   ( 
   struct _dij_machine *self, 
   void *fnode, 
   void (*send)(struct iChannel *self, struct _stack_member *data),
   int left 
   )
   {
   printf("invoke\n");
   /*this does all the leg work of abandoning control to another machine*/
   struct iProcess *p;
   /*create an iProcess*/
   p = self->fgraph->load( self->fgraph, fnode, &(self->wait_channel), left );
   /*this is what I want done with the result*/
   printf("...load\n");
   self->wait_channel.send = send;
   /*detach the current process*/
   self->process->detach( self->process );
   printf("...detach\n");
   /*attach the new process*/
   p->attach(p);
   printf("...attach\n");
   /*put this machine into waiting mode*/
   self->mode = MODE_WAIT;
   }

/*this structure is for gathering the material for an fnode apply operation*/
struct _hot_call
   {
   void *fnode;
   struct _parameter *p;
   };

struct _hot_call *new_hot_call( void *fnode )
   {
   struct _hot_call *ret = (struct _hot_call *)malloc(sizeof(struct _hot_call));
   ret->fnode = fnode;
   ret->p = 0;
   return ret;
   }

void add_parameter( struct _hot_call *f, struct _parameter *p )
   {
   struct _parameter *sploop;
   if( f->p == 0 ) { f->p = p; return; }
   sploop = f->p;
   while(sploop->next != 0 ) sploop = sploop->next;
   sploop->next = p;
   }

void apply( struct _hot_call *f, struct _object o )
   {
   struct _parameter *newp;
   newp = (struct _parameter *)malloc(sizeof(struct _parameter));
   newp->value = o;
   newp->curry_flag = 0;
   newp->next = 0;
   add_parameter( f, newp );
   }

void curry( struct _hot_call *f )
   {
   struct _parameter *newp;
   newp = (struct _parameter *)malloc(sizeof(struct _parameter));
   newp->curry_flag = -1;
   newp->next = 0;
   add_parameter( f, newp );
   }

/*end hot call*/

int *variable_address( struct _dij_machine *M, int code )
   {
   int i = 0;
   int j = 0;
   printf("searching variables: \nparameters\n");
   for( i = 0; i < M->context->num_parameters; i++ )
      {
      printf("%d\n", j);
      if( M->context->namespace[j] == code ) return M->variables+j;
      j++;
      }
   printf("locals\n");
   for( i = 0; i < M->context->num_locals; i++ )
      {
      printf("%d\n", j);
      if( M->context->namespace[j] == code ) return M->variables+j;
      j++;
      }
   printf("returns\n");
   for( i = 0; i < M->context->num_returns; i++ )
      {
      printf("%d\n", j);
      if( M->context->namespace[j] == code ) return M->variables+j;
      j++;
      }
   }

char *print_general_error( struct iException *self )
   {
   return (char *)(self->E);
   }

void general_error( struct _dij_machine *M, char *message )
   {
   M->error_condition.E = message;
   M->error_condition.message = print_general_error; 
   M->mode = MODE_FAULT;
   }

/*instructions*/

void dij_machine_ichannel_send(struct iChannel *self, struct _stack_member *data);

void inst_check( struct _dij_machine *M )
{
   struct _object o;
   struct _hot_call *hc;
   struct _parameter *sploop;
   int to_invoke = 0;
   void *fnode = 0;
   if( !(M->stack) ) { return; }
   o = pop( &(M->stack) );
   if(o.type == &TYPE_HOT_CALL)
      {
      printf("evaluate\n");
      hc = (struct _hot_call *)(o.value);
      sploop = hc->p;
      if(sploop)
         {
         /*dispose of the special case in which we are calling a function without parameters*/
         /*TODO this is some messy case analysis here*/
         if( !sploop->next && sploop->curry_flag ) { to_invoke = -1; fnode = hc->fnode; }
         while(sploop->next) 
            {
            printf("\t%x\n", sploop->value.value);
            if(sploop->next->next) {sploop = sploop->next; }
            else 
               {
               if(sploop->next->curry_flag) 
                  {
                  to_invoke = -1; 
                  free(sploop->next); 
                  sploop->next = 0; 
                  } else { to_invoke = 0; break; }
               }
            }
         }
      if( !fnode )
         {
         if( hc->fnode )
            {
            printf("apply %x, %x, %x\n", M, hc, M->fgraph);
            fnode = M->fgraph->apply(M->fgraph, hc->fnode, hc->p);
            } else {
            fnode = M->fgraph->fcurry(M->fgraph, hc->p);
            }
         }
      printf("exit apply %x\n", M->fgraph->is_loadable);
      if( M->fgraph->is_loadable(M->fgraph, fnode, 0) && to_invoke )
         {
         invoke( M, fnode, dij_machine_ichannel_send, 0 );
         } else {
         push( &(M->stack), object( (int)fnode, &TYPE_F_NODE ) );
         }

      } else { printf("dont evaluate\n"); push( &(M->stack), o ); };
}

void inst_push( struct _dij_machine *M )
{
   int value;
   M->inst_pointer = M->inst_pointer+1;
   value = M->code[M->inst_pointer];
   push( &(M->stack), object( value, &TYPE_SCALAR ) );
   printf("push %x\n", value);
}

void inst_apply( struct _dij_machine *M )
{
   struct _object func;
   struct _object param;
   struct _hot_call *p;
   printf("apply\n");
   param = pop( &(M->stack) );
   printf("param = %x", param.value);
   func = pop( &(M->stack) );
   printf("func = %x", func.value);
   if(func.type == &TYPE_HOT_CALL)
   {
   printf("extend hot call\n");
   p = (struct _hot_call *)(func.value);
   } else {
   printf("create hot call\n");
   p = new_hot_call((void *)(func.value));
   printf("copy\n");
   }
   apply( p, param );
   printf("apply\n");
   push( &(M->stack), object((int)p, &TYPE_HOT_CALL) );
   printf("complete apply\n");
}

void inst_curry( struct _dij_machine *M )
{
   struct _object func;
   struct _hot_call *p;
   printf("curry\n");
   func = pop( &(M->stack) );
   if( func.type == &TYPE_HOT_CALL )
   {
      printf("hot curry\n");
      curry( (struct _hot_call *)(func.value) );
   } else {
      printf("cold curry\n");
      p = new_hot_call( (void *)(func.value ) );
      func = object ( (int)p, &TYPE_HOT_CALL );
      curry( p );
   }
   push( &(M->stack), func);
   printf("complete curry\n");
}

void inst_halt( struct _dij_machine *M )
{
   struct _stack_member *ret = 0;
   printf("halt\n");
   M->mode = MODE_HALT;
}

void inst_reference_variable( struct _dij_machine *M )
{
   int ref;
   printf("referencing variable\n");
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   printf("...  %d\n", ref);
   push( &(M->stack), object( *(variable_address(M, ref)), &TYPE_SCALAR ) );
}

int globals[1024];
int max_global = -1;

void inst_reference_global( struct _dij_machine *M )
{
   int ref;
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   push( &(M->stack), object(globals[ref], &TYPE_SCALAR ) );
}

void lvalue_name_liquidate( void *address, struct _parameter *parameters, struct _dij_machine *M )
{
   int value = parameters->value.value;
   int *target = (int *)address;
   struct _parameter *new_parameter;
   printf("liquidating name %x\n", address );
   *target = value;
   /*parameters->consumed_flag = -1;*/
   printf("success\n");
   while( parameters )
      {
      new_parameter = parameters;
      printf(":-( %x %x\n", new_parameter, new_parameter->next );
      parameters = parameters->next;
      free(new_parameter);
      }
}

void inst_lvalue_name( struct _dij_machine *M )
{
   int ref;
   struct _lvalue *lvalue = (struct _lvalue *)malloc(sizeof(lvalue));
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   lvalue->liquidate = lvalue_name_liquidate;
   lvalue->parameter = variable_address(M, ref );
   push(&(M->stack), object( (int)lvalue, &TYPE_LVALUE ) );
   printf("lvalue named\n");
}

void inst_assign( struct _dij_machine *M )
{
   struct _stack_member *lvalue_stack = 0;
   struct _object sploop;
   struct _lvalue *lvalue;
   struct _parameter *parameters = 0;
   struct _parameter *new_parameter;
   sploop = pop( &(M->stack) );
   while( sploop.type != &TYPE_LVALUE ) {
      new_parameter = (struct _parameter *)malloc(sizeof(struct _parameter) );
      new_parameter->next = parameters;
      parameters = new_parameter;
      parameters->curry_flag = 0;
      parameters->value = sploop;
      printf("assign %x\n", sploop.value);
      sploop = pop( &(M->stack) );
      }
   while( M->stack && sploop.type == &TYPE_LVALUE )
      {
      push( &lvalue_stack, sploop);
      sploop = pop( &(M->stack) );
      }
   if( sploop.type != &TYPE_LVALUE) 
      {
      push( &(M->stack), sploop );
      } else {
      push( &lvalue_stack, sploop );
      }
   while( lvalue_stack )
      {
      lvalue = (struct _lvalue *)pop(&lvalue_stack).value;
      lvalue->liquidate( lvalue->parameter, parameters, M );
      printf("back out at assignment\n");
      free(lvalue);

      printf("assignment\n");
      }
   printf("assignment complete\n");
}

void inst_add( struct _dij_machine *M )
{
  int left, right;
  left = pop( &(M->stack) ).value;
  right = pop( &(M->stack) ).value;
  push( &(M->stack), object( left+right, &TYPE_SCALAR ) );
  printf("addition\n");
}

void bit_bucket(struct iChannel *self, struct _stack_member *data)
   {
   /*this is the send function for an empty output channel, it just frees the stack data*/
   struct _stack_member *kill_me;
   struct _stack_member *sploop;
   struct _dij_machine *m = ((struct _dij_machine *)(self->C));
   sploop = data;
   printf("dumped to bit bucket\n");
   while( sploop )
      {
      kill_me = sploop;
      sploop = sploop->under;
      free(kill_me);
      }
   m->process->attach(m->process);
   }

void lvalue_call_liquidate( void *address, struct _parameter *rvalues, struct _dij_machine *M )
   {
   void *fnode;
   struct _hot_call *hc = (struct _hot_call *)address;
   struct _parameter *p;
   struct _parameter *prev;
   struct _parameter *sploop;
   prev = 0;
   p = hc->p;
   while( p->next ) 
      { 
      prev = p;
      p = p->next; 
      }
   sploop = rvalues;
   while( sploop->next ) { sploop = sploop->next; }
   if(prev)
      {
      free(prev->next);
      prev->next = 0;
      fnode = M->fgraph->apply( M->fgraph, hc->fnode, hc->p );
      fnode = M->fgraph->apply( M->fgraph, fnode, rvalues );
      } else {
      hc->p = 0;
      free(hc->p);
      fnode = M->fgraph->apply( M->fgraph, hc->fnode, rvalues );
      }
   invoke( M, fnode, bit_bucket, 1 ); 
   }
/*
this is the previous implementation
   {
   /*address is presumed to be a partial call
     the name of the game is to apply the rvalues to it and invoke-/
   struct _partial_call *pc = (struct _partial_call *)address;
   struct _stack_member *m;
   half_execution_mechanism exec;
   struct _parameter *sploop;
   /*printf("??? %x\n", pc->first_param);
     printf("attempting to liquidate lvalue %x %x\n", pc, pc->first_param->value );-/
   exec = pc->left->exec; /*((struct _execution_pair *)(pc->first_param->value.value))->left;-/
   printf("did this work?\n");
   sploop = pc->first_param;
   printf("yep %x\n", sploop);
   while(sploop && sploop->next && !(sploop->next->curry_flag) )
      {
      sploop = sploop->next;
      printf("liquidating %x\n", sploop->value.value);
      }
   if( sploop && !(sploop->curry_flag) ) { sploop->next = rvalues; }
   else         { pc->first_param = rvalues; }
   sploop = pc->first_param;
   while(sploop)
     {
       printf("(lvalue call) %x %d\n", sploop->value.value, sploop->curry_flag );
       sploop = sploop->next;
     }
   printf("try here\n");
   exec( pc->left, pc->first_param, M); 
   printf("back out at liquidate\n");

   }
*/

void inst_lvalue_call( struct _dij_machine *M )
{
/*
the object on top of the stack had better be a hot call, because we are going to treat it like one.
*/
   int pc = pop( &(M->stack) ).value;
   struct _lvalue *lv = (struct _lvalue *)malloc(sizeof(struct _lvalue) );
   printf("lvalue call\n");
   lv->parameter = (void *)pc;
   lv->liquidate = lvalue_call_liquidate;
   push( &(M->stack), object( (int)lv, &TYPE_LVALUE ) );
}

void inst_anonymous_parameter( struct _dij_machine *M )
{
   struct _object v;
   if(M->anonymous_in_ptr >= 
      (
      M->context->num_parameters+
      M->context->num_returns+
      M->context->num_locals + 
      M->context->num_anonymous
      ) ) { general_error( M, "overused anonymous inputs" ); }
   v.value = M->variables[M->anonymous_in_ptr]; 
   v.type = M->types[M->anonymous_in_ptr];
   M->anonymous_in_ptr = M->anonymous_in_ptr + 1;
   printf("anonymous parameter %x\n", v.value);
   push( &(M->stack), v );
   printf("fetched anonymous\n");
}

void anonymous_return_liquidate( void *address, struct _parameter *rvalues, struct _dij_machine *M )
{
   /*address is meaningless, send the rvalues out on the anonymous output channel*/
   struct _stack_member *head = 0;
   struct _stack_member *s_sploop;
   struct _parameter *p_sploop;
   p_sploop = rvalues;
   while(p_sploop)
      {
      if( !head )
         {
         head = (struct _stack_member *)malloc(sizeof(struct _stack_member));
         s_sploop = head;
         } else {
         s_sploop->under = (struct _stack_member *)malloc(sizeof(struct _stack_member));
         s_sploop = s_sploop->under;
         }
      s_sploop->under = 0;
      s_sploop->value = p_sploop->value;
      p_sploop = p_sploop->next;
      }
   M->anonymous_out->send( M->anonymous_out, head );
}

void inst_anonymous_return( struct _dij_machine *M )
{
   struct _lvalue *lv = (struct _lvalue *)malloc(sizeof(struct _lvalue) );
   lv->parameter = 0;
   lv->liquidate = anonymous_return_liquidate;
   push( &(M->stack), object( (int)lv, &TYPE_LVALUE ) );
}

void inst_spawn( struct _dij_machine *M )
   {
   /*this will be similar to invoke but has no control over the parent process
     the fact that this instruction is pretty trivial and invoke has to create
     a channel and manage the child process is kind of stinky*/
   void *fnode_in;
   void *fnode_out;
   fnode_in = (void *)(pop( &(M->stack) ).value);
   fnode_out = M->fgraph->spawn(M->fgraph, fnode_in);
   push( &(M->stack), object((int)fnode_out, &TYPE_F_NODE ) );
   }

/*this is the previous implementation of this instruction.
All of this logic should end up in the control module*/
/*
{
   /*on the stack is a partial call, it will be provided with the downstream 
     bridgehead as its first parameter, then loaded into its own machine to execute.-/
   struct _bidirectional_bridge *bridge = malloc(sizeof(struct _bidirectional_bridge) );
   struct _function_metadata *left;
   struct _function_metadata *right; 
   struct _partial_call *pc;
   struct _partial_call *upstream_end;
   struct _partial_call *downstream_end;
   struct _machine *M;
   struct _parameter *sploop;
   left = new_function_metadata(send_downstream_execution_mechanism, (void *)bridge);
   right = new_function_metadata(receive_upstream_execution_mechanism, (void *)bridge);
   pc = copy_partial_call( (struct _partial_call *)(pop( &(M0->stack) ).value) );
   upstream_end = initialize_partial_call( left, right , 0 );
   downstream_end = initialize_partial_call( new_function_metadata(send_upstream_execution_mechanism, bridge), 
								   new_function_metadata(receive_downstream_execution_mechanism, bridge),0 ); 

   M = new_machine();
   sploop = pc->first_param;
   while( sploop && !( sploop->curry_flag ) ) { sploop = sploop->next; }
   if( !sploop ) { sploop = malloc(sizeof( struct _parameter )); add_param(pc, sploop ); }
   sploop->curry_flag = 0;
   sploop->value = object( (int)downstream_end, TYPE_SCALAR );
   if( !(sploop->next) ) curry( pc );
   push( &(M0->stack), object( (int)upstream_end, TYPE_SCALAR ) );

   bridge->upstream.first_client = bridge->upstream.last_client = 0;
   bridge->downstream.first_client = bridge->downstream.last_client = 0;
   bridge->upstream.first_message = bridge->upstream.last_message = 0;
   bridge->downstream.first_message = bridge->downstream.last_message = 0;
   M->exit_procedure = channel_exit_procedure;
   M->exit_datum = &(bridge->upstream);
   evaluate_partial_call(pc, M);
   M->mode = MODE_HALT;

}
*/

/*this instruction is at the top of if and do loops to indicate that the value following is just a word of data and not an instruction*/
void inst_noop( struct _dij_machine *M )
  {
  printf("hit noop %d\n", M->code[M->inst_pointer+1]);
  M->inst_pointer = M->inst_pointer+1;
  }

/*the tests for if and do seem to be exactly the same*/
void inst_test( struct _dij_machine *M )
  {
  /*in a test, if the value is false (even) we jump forward to the next marker, otherwise we continue*/
  int mark; /*tests include a distance to the next marker*/
  int value = pop(&(M->stack)).value;
  M->inst_pointer = M->inst_pointer+1;
  mark = M->code[M->inst_pointer];
  if(value%2) { printf("not jumping\n"); } else { M->inst_pointer = M->inst_pointer+mark; printf("jumping %d\n", mark); } 
  }

void inst_if_mark( struct _dij_machine *M )
  {
  /*at an if marker the machine will get the exit value from the header and jump out*/
  int head; /*markers include a distance back to the header*/
  int end;
  M->inst_pointer = M->inst_pointer+1;
  head = M->code[M->inst_pointer];
  printf("hit if marker %d\n", head);
  end = M->code[M->inst_pointer - head ];
  M->inst_pointer = M->inst_pointer - head + end - 1;
  }

void inst_do_mark( struct _dij_machine *M )
  {
  /*at a do marker the machine will return to the header*/
  int head; /*markers include a distance back to the header*/
  M->inst_pointer = M->inst_pointer+1;
  head = M->code[M->inst_pointer];
  printf("hit do marker %d\n", head );
  M->inst_pointer = M->inst_pointer - head;
  }

void inst_left_join( struct _dij_machine *M )
   {
   void *fnode_right;
   void *fnode_left;
   void *fnode_out;
   fnode_right = (void *)(pop( &(M->stack) ).value);
   fnode_left = (void *)(pop( &(M->stack) ).value);
   fnode_out = M->fgraph->ljoin(M->fgraph, fnode_left, fnode_right);
   push( &(M->stack), object((int)fnode_out, &TYPE_F_NODE ) );
   }
  /*pervious implementation of inst_left_join*/
  /*
  {
  struct _partial_call *right = (struct _partial_call *)(pop(&(M->stack) ).value);
  struct _partial_call *left = (struct _partial_call *)(pop(&(M->stack) ).value);
  struct _function_metadata *left_meta = copy_function_metadata( left->right );
  struct _function_metadata *right_meta = copy_function_metadata( right->right );
  left_meta->exec = partial_call_execution_mechanism_left;
  right_meta->exec = partial_call_execution_mechanism_right;
  left_meta->code = left;
  right_meta->code = right;
  printf("left join\n");
  push( &(M->stack), object( (int)initialize_partial_call( left_meta, right_meta, 0 ), TYPE_SCALAR ) );
  }
*/

void inst_namespace_join( struct _dij_machine *M )
{
  void *second = (void *)(pop(&(M->stack)).value);
  void *first = (void *)(pop(&(M->stack)).value); 
  void *ret;
  ret = M->fgraph->njoin(M->fgraph, first, second);
  push( &(M->stack), object((int)ret, &TYPE_F_NODE) );
}

void inst_not( struct _dij_machine *M )
{
  int arg;
  arg = pop( &(M->stack) ).value;
  push( &(M->stack), object( ~arg, &TYPE_SCALAR ) );
  printf("bitwise not\n");
}

void inst_neg( struct _dij_machine *M )
{
  int arg;
  arg = pop( &(M->stack) ).value;
  push( &(M->stack), object( -arg, &TYPE_SCALAR ) );
  printf("twos compliment negation\n");
}

void inst_equal( struct _dij_machine *M )
{
   int a;
   int b;
   a = pop( &(M->stack) ).value;
   b = pop( &(M->stack) ).value;
   if( a==b ) { push( &(M->stack), object( -1, &TYPE_SCALAR ) ); }
   else { push( &(M->stack), object( 0, &TYPE_SCALAR ) ); }
}

/*end instructions*/

/*debugging functions*/

void print_stack( struct _dij_machine *M )
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

void machine_list( struct _dij_machine *M )
   {
/*TODO implement debugging features in the control system*/
/*   struct _machine *sploop;
   sploop = first_machine;
   while( sploop )
      {
      printf("\tmachine_list: %x %d\n", sploop, sploop->mode);
      sploop = sploop->next;
      }*/
   }

void constants( struct _dij_machine *M )
   {
   int i;
   for(i = 0; i <= max_global; i++ )
      {
      printf("\tconstants: %x\n", globals[i]);
      }
   }

void variables( struct _dij_machine *M )
   {
   int i;
   int j = 0;
   struct _fcontext *md = M->context;
   printf("\tvariables parameters\n");
   for(i = 0; i < md->num_parameters; i++ )
      {
      printf("\tvariables %d:%x\n", md->namespace[i], M->variables[j]);
      j++;
      }
   printf("\tvariables locals\n");
   for(i = 0; i < md->num_locals; i++ )
      {
      printf("\tvariables %d:%x\n", md->namespace[i+md->num_parameters], M->variables[j]);
      j++;
      }
   printf("\tvariables returns\n");
   for(i = 0; i < md->num_returns; i++ )
      {
      printf("\tvariables %d:%x\n", md->namespace[i+md->num_parameters+md->num_locals], M->variables[j]);
      j++;
      }
   }

void partial_call( struct _dij_machine *M )
   {
     /*TODO implement debugging functions in the control structure*/
     /*the object on the top of the stack is a _partial_call, which I will inspect*/
     /*
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
   */
   }

instruction debugs[] =
   {
   print_stack,
   machine_list,
   constants,
   variables,
   partial_call
   };

void debug( struct _dij_machine *M )
   {
   int ref;
   instruction exec;
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   exec = debugs[ref];
   exec(M);
   }
/*end debugging functions*/

/*this is the implementation of the iCode and iMachine interfaces to this machine*/

void dij_imachine_destroy( struct iMachine *self )
   {
   struct _dij_machine *killme = (struct _dij_machine *)(self->M);
   struct _stack_member *sploop = killme->stack;
   struct _stack_member *delete;
   while( sploop )
      {
      delete = sploop;
      sploop = sploop->under;
      free( delete );
      };

   free( killme );
   /*SPEC the iMachine->destroy method should free self*/
   free( self );
   }

struct iException *dij_imachine_run( struct iMachine *self )
{
   struct _dij_machine *M = self->M;
   M->mode = MODE_RUN;
   while(M->mode == MODE_RUN)
      {
      instruction I;
      printf(" ... %x . %d\n", M->code, M->inst_pointer );
      I = (instruction)(M->code[M->inst_pointer]);
      I(M);
      M->inst_pointer = M->inst_pointer+1;
      }
   if(M->mode == MODE_HALT) {printf("dij_machine halting\n"); return (struct iException *)0;}
   if(M->mode == MODE_WAIT) {printf("dij_machine waiting\n"); return (struct iException *)-1;}
   if(M->mode == MODE_FAULT) {return &(M->error_condition);}
   general_error(M, "there has been a MYSTERIOUS MISTAKE with the dij machine:"
                    " it is neither in RUN, nor HALT, nor WAIT, nor FAULT modes" );
   return &(M->error_condition);
}

void dij_machine_ichannel_send(struct iChannel *self, struct _stack_member *data)
   {
   struct _dij_machine *m = ((struct _dij_machine *)(self->C));
   graft( &(m->stack), data );
   m->process->attach(m->process);
   }

struct iMachine *dij_icode_new
   (
   struct iCode *self,
   struct _fcontext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
   )
   {
   struct iMachine *ret = (struct iMachine *)malloc(sizeof(struct iMachine));
   struct _dij_machine *M = (struct _dij_machine *)malloc(sizeof(struct _dij_machine));
   ret->M = M;

   ret->run = dij_imachine_run;
   ret->destroy = dij_imachine_destroy;

   process->get_memory( process, &(M->variables), &(M->types), &(M->anonymous_out) );

   M->process = process;
   M->context = context;
   M->stack = 0;
   M->inst_pointer = 0;
   M->code = self->C;
   M->anonymous_in_ptr = 
      (context->num_parameters + context->num_locals + context->num_returns); 

   M->wait_channel.C = M;
   M->wait_channel.send = dij_machine_ichannel_send;

   M->mode = MODE_UNINITIALIZED;
   M->fgraph = fgraph;
   return ret;

   }

struct iCode *dij_box( int *code  )
   {
   struct iCode *ret = (struct iCode *)malloc(sizeof(struct iCode));
   ret->C = code;
   ret->new = dij_icode_new;
   return ret;
   }
