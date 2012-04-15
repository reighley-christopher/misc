struct _function_codeblock *new_function_codeblock( int *instructions )
   {
   struct _function_codeblock *ret = malloc(sizeof(struct _function_codeblock));
   printf("new_function_codeblock %x\n", ret);
   ret->next = 0; ret->last = 0; ret->instructions = instructions;
   return ret;
   }

struct _function_codeblock *copy_function_codeblock( struct _function_codeblock *old )
   {
   struct _function_codeblock *ret;
   printf("copy_function_codeblock %x\n", old);
   if( !old ) { printf("copying 0\n"); return 0; }
   ret = new_function_codeblock( old->instructions );
   printf("finish new\n");
   if( old->next != 0 ) { ret->next = copy_function_codeblock( old->next ); } 
   printf("finish copy\n");
   return ret;
   }

void append_function_codeblock( struct _function_codeblock *one, struct _function_codeblock *two )
   {
   struct _function_codeblock *sploop = one;
   if( !one || !two ) { printf("error, codeblocks are null\n"); } 
   while( sploop->next ) { sploop = sploop->next; }
   sploop->next = two;
   two->last = sploop;
   }

int *array_copy( int *array, int size );

struct _function_metadata *new_function_metadata( half_execution_mechanism exec, void *code )
{
   struct _function_metadata *f;
   f = malloc(sizeof(struct _function_metadata));
   f->exec = exec;
   f->code = code;
   f->num_parameters = 0;
   f->parameters = 0;
   f->num_locals = 0;
   f->locals = 0;
   f->num_returns = 0;
   f->returns = 0;
   return f;
}

struct _function_metadata *copy_function_metadata( struct _function_metadata *copy )
{
   struct _function_metadata *ret;
   ret = (struct _function_metadata *)malloc(sizeof( struct _function_metadata ) );
   ret->num_parameters = copy->num_parameters;
   ret->num_locals = copy->num_locals;
   ret->num_returns = copy->num_returns;
   ret->parameters = array_copy( copy->parameters, copy->num_parameters );
   ret->locals = array_copy( copy->locals, copy->num_locals ); 
   ret->returns = array_copy( copy->returns, copy->num_returns );
   ret->up = copy->up;
   ret->exec = copy->exec;
   ret->code = copy->code;
   return ret;
}

void add_param( struct _partial_call *pc, struct _parameter *p )
{
   p->next = 0;
   p->consumed_flag = 0;
   if( !(pc->first_param) ) { pc->first_param = p; }
   if( pc->last_param ) { pc->last_param->next = p; }
   pc->last_param = p;
   pc->params = pc->params+1;
   printf("done\n");
}

void apply( struct _partial_call *pc, struct _object v )
{
   struct _parameter *p = (struct _parameter *)malloc(sizeof(struct _parameter) );
   add_param( pc, p );
   p->value = v;
   p->curry_flag = 0;
   printf("apply value: %x to %x\n", v.value, pc ); 
}

void curry( struct _partial_call *pc )
{
   struct _parameter *p = (struct _parameter *)malloc(sizeof(struct _parameter) );
   add_param( pc, p );
   p->curry_flag = -1;
}

struct _partial_call *new_partial_call( )
   {
   struct _partial_call *ret;
   ret = (struct _partial_call *)malloc(sizeof( struct _partial_call ) );
   ret->first_param = 0;
   return ret;
   }

struct _partial_call *initialize_partial_call( struct _function_metadata *left, struct _function_metadata *right, int count, ... )
   {
   va_list params;
   int i;
   struct _partial_call *ret = new_partial_call();
   ret->left = left;
   ret->right = right;
   va_start( params, count );
   for(i = 0; i < count; i++) { apply( ret, va_arg( params, struct _object ) ); };
   printf("INIT.PART.CALL %x %x %x %d\n", ret, left, right, left->num_parameters); 
   return ret;
   }

struct _partial_call *copy_partial_call( struct _partial_call *old )
   {
   struct _partial_call *ret = new_partial_call();
   struct _parameter *param;
   ret->left = old->left;
   ret->right = old->right;
   ret->params = old->params;
   ret->first_param = 0;
   ret->last_param = 0;
   param = old->first_param;
   while( param )
      {
      
      if( !(ret->first_param) ) 
         { 
         ret->last_param = (struct _parameter *)malloc(sizeof( struct _parameter) );
         ret->first_param = ret->last_param;
         } else { 
         ret->last_param->next = (struct _parameter *)malloc(sizeof( struct _parameter) );
         ret->last_param = ret->last_param->next;
         }
      ret->last_param->value = param->value;
      ret->last_param->curry_flag = param->curry_flag;
      param = param->next;
      printf("*");
      }
   return ret;
   }

void destroy_partial_call( struct _partial_call *pc )
{
   struct _parameter *p;
   while( pc->first_param != 0 )
      {
      p = pc->first_param;
      pc->first_param = pc->first_param->next;
      free( p );
      }
}

int may_partial_call_execute( struct _partial_call *pc )
{
   /*the conditions in which a _partial_call may be evaluated are :
     the metadata must have a mechanism and code on both the left and the right.
     there must be at least one curried term.
     the last non curried term must come before the first curried term.
   */
   int first_curried_term = 0;
   int last_non_curried_term = 0; 
   int index = 0;
   struct _parameter *p = pc->first_param;
   while( p )
   {
   index++;
   if( p->curry_flag && !first_curried_term ) 
      { first_curried_term = index; }
   if( !(p->curry_flag) ) 
      { last_non_curried_term = index; }
   p = p->next;
   }  
   printf("first curried term : %d, last noncurried term: %d\n, pc %x, pc->left %x, pc->right %x\n",
	  first_curried_term, last_non_curried_term, pc, pc->left, pc->right );
   return ( 
          first_curried_term > last_non_curried_term && 
          first_curried_term > 0 && 
          pc->left->exec && pc->right->exec && pc->left->code && pc->right->code 
          );
}

void evaluate_partial_call( struct _partial_call *pc, struct _machine *M )
{
    /*we will only be executing this if we have terminated it, and all
      the parameters are filled*/
   struct _stack_member *m;
   struct _function_metadata *left;
   struct _function_metadata *right;
   half_execution_mechanism exec;
   printf("evaluate partial call %d\n", may_partial_call_execute( pc) );
   if( may_partial_call_execute( pc )  )
   {
      printf("go ahead and run\n");
      exec = pc->right->exec;
      exec( pc->right, pc->first_param, M );
   } else {
     printf("wtf?!!?\n");
   /*otherwise we will store the partial call, the code for which is the
     present partial call, and the calling convention evaluates partial 
     calls.  The metadata will be the same as the parent, except that some
     the parameters will have been changed into local variables*/

     right = copy_function_metadata( pc->right );
     left = copy_function_metadata( pc->left );
     right->exec = partial_call_execution_mechanism_right;
     left->exec = partial_call_execution_mechanism_left;
     right->code = pc;
     left->code = pc;
     printf("push partial call\n");
     push( &(M->stack), object( (int)initialize_partial_call( left, right, 0 ), TYPE_SCALAR ) );
   }
   printf("finish evaluate\n");
}
