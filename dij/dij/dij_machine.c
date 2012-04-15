struct _dij_machine
   {
   char mode; 
   struct _stack_member *stack;
   int *code;
   int inst_pointer;
   int *variables;
   int *types;
   struct _function_metadata *context;
   struct _parameter *next_anonymous_parameter;
   struct _stack_member **last_anonymous_return;
   };

void dij_execution_mechanism( struct _function_metadata *f, struct _parameter *p, struct _machine *M0 )
{
   struct _machine *M = new_machine();
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
   /*
   run_machine(M);
   */
}

struct _machine *new_machine( )
   {
   struct _dij_machine *ret = (struct _machine *)malloc(sizeof(struct _machine) );
   struct _machine *sploop;
   ret->stack = 0;
   ret->mode = MODE_UNINITIALIZED;
   ret->variables = 0;
   ret->last_anonymous_return = 0;
   ret->exit_procedure = 0;
   ret->exit_datum = 0;
   if( !first_machine ) first_machine = ret;
   if( last_machine ) machine_add_after( last_machine, ret );
   last_machine = ret;
        sploop = first_machine;
        while( sploop )
        {
        printf("\tmachine_list: %x %d\n", sploop, sploop->mode);
        sploop = sploop->next;
        }
   return ret;
   }

void destroy_machine( struct _machine *killme )
   {
   struct _stack_member *sploop = killme->stack;
   struct _stack_member *delete;
   while( sploop )
      {
      delete = sploop;
      sploop = sploop->under;
      free( delete );
      };
   if( killme->variables ) free( killme->variables );
   free( killme );
   }

int *variable_address( struct _machine *M, int code )
   {
   int i = 0;
   int j = 0;
   printf("searching variables: \nparameters\n");
   for( i = 0; i < M->context->num_parameters; i++ )
      {
      printf("%d\n", j);
      if( M->context->parameters[i] == code ) return M->variables+j;
      j++;
      }
   printf("locals\n");
   for( i = 0; i < M->context->num_locals; i++ )
      {
      printf("%d\n", j);
      if( M->context->locals[i] == code ) return M->variables+j;
      j++;
      }
   printf("returns\n");
   for( i = 0; i < M->context->num_returns; i++ )
      {
      printf("%d\n", j);
      if( M->context->returns[i] == code ) return M->variables+j;
      j++;
      }
   }

void run_machine( struct _machine *M )
{
   while(M->mode == MODE_RUN)
      {
      instruction I;
      printf(" ... %x . %d\n", M->code, M->inst_pointer );
      I = (instruction)(M->code[M->inst_pointer]);
      I(M);
      M->inst_pointer = M->inst_pointer+1;
      }
}

/*instructions*/

void inst_check( struct _machine *M )
{
   struct _object o;
   if( !(M->stack) ) { return; }
   o = pop( &(M->stack) );
   if(o.type == TYPE_HOT_CALL)
   {
   printf("evaluate\n");
   evaluate_partial_call( (struct _partial_call*)(o.value), M  );
   } else { printf("dont evaluate\n"); push( &(M->stack), o ); };
}

void inst_push( struct _machine *M )
{
   int value;
   M->inst_pointer = M->inst_pointer+1;
   value = M->code[M->inst_pointer];
   push( &(M->stack), object( value, TYPE_SCALAR ) );
   printf("push %x\n", value);
}

void inst_apply( struct _machine *M )
{
   struct _object func;
   struct _object param;
   struct _partial_call *p;
   printf("apply\n");
   param = pop( &(M->stack) );
   printf("param = %x", param.value);
   func = pop( &(M->stack) );
   printf("func = %x", func.value);
   if(func.type == TYPE_HOT_CALL)
   {
   printf("extend hot call\n");
   p = (struct _partial_call *)(func.value);
   } else {
   printf("create hot call\n");
   p = copy_partial_call((struct _partial_call *)(func.value));
   printf("copy\n");
   }
   apply( p, param );
   printf("apply\n");
   push( &(M->stack), object((int)p, TYPE_HOT_CALL) );
   printf("complete apply\n");
}

void inst_curry( struct _machine *M )
{
   struct _object func;
   struct _partial_call *p;
   printf("curry\n");
   func = pop( &(M->stack) );
   if( func.type == TYPE_HOT_CALL )
   {
      printf("hot curry\n");
      curry( (struct _partial_call *)(func.value) );
   } else {
      printf("cold curry\n");
      p = copy_partial_call( (struct _partial_call *)(func.value ) );
      func = object ( (int)p, TYPE_HOT_CALL );
      curry( p );
   }
   push( &(M->stack), func);
   printf("complete curry\n");
}

void inst_halt( struct _machine *M )
{
   struct _stack_member *ret = 0;
   printf("halt\n");
   M->mode = MODE_HALT;
   M->exit_procedure( M, M->exit_datum );
}

void inst_reference_variable( struct _machine *M )
{
   int ref;
   printf("referencing variable\n");
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   printf("...  %d\n", ref);
   push( &(M->stack), object( *(variable_address(M, ref)), TYPE_SCALAR ) );
}

int globals[1024];
int max_global = -1;

void inst_reference_global( struct _machine *M )
{
   int ref;
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   push( &(M->stack), object(globals[ref], TYPE_SCALAR ) );
}

void lvalue_name_liquidate( int address, struct _parameter *parameters, struct _machine *M )
{
   int value = parameters->value.value;
   int *target = (int *)address;
   printf("liquidating name %x\n", address );
   *target = value;
   parameters->consumed_flag = -1;
   printf("success\n");
}

void inst_lvalue_name( struct _machine *M )
{
   int ref;
   struct _lvalue *lvalue = malloc(sizeof(lvalue));
   M->inst_pointer = M->inst_pointer+1;
   ref = M->code[M->inst_pointer];
   lvalue->liquidate = lvalue_name_liquidate;
   lvalue->parameter = (int)variable_address(M, ref );
   push(&(M->stack), object( (int)lvalue, TYPE_LVALUE ) );
   printf("lvalue named\n");
}

void inst_assign( struct _machine *M )
{
   struct _stack_member *lvalue_stack = 0;
   struct _object sploop;
   struct _lvalue *lvalue;
   struct _parameter *parameters = 0;
   struct _parameter *new_parameter;
   sploop = pop( &(M->stack) );
   while( sploop.type != TYPE_LVALUE ) {
      new_parameter = (struct _parameter *)malloc(sizeof(struct _parameter) );
      new_parameter->next = parameters;
      parameters = new_parameter;
      parameters->curry_flag = 0;
      parameters->consumed_flag = 0;
      parameters->value = sploop;
      printf("assign %x\n", sploop.value);
      sploop = pop( &(M->stack) );
      }
   while( M->stack && sploop.type == TYPE_LVALUE )
      {
      push( &lvalue_stack, sploop);
      sploop = pop( &(M->stack) );
      }
   if( sploop.type != TYPE_LVALUE) 
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
      while( parameters && parameters->consumed_flag )
         {
         new_parameter = parameters;
         printf(":-( %x %x\n", new_parameter, new_parameter->next );
         parameters = parameters->next;
         free(new_parameter);
         }
      printf("assignment\n");
      }
   printf("assignment complete\n");
}

void inst_add( struct _machine *M )
{
  int left, right;
  left = pop( &(M->stack) ).value;
  right = pop( &(M->stack) ).value;
  push( &(M->stack), object( left+right, TYPE_SCALAR ) );
  printf("addition\n");
}

void lvalue_call_liquidate( int address, struct _parameter *rvalues, struct _machine *M )
   {
   /*address is presumed to be a partial call
     the name of the game is to complete it by filling out its stack and*/
   struct _partial_call *pc = (struct _partial_call *)address;
   struct _stack_member *m;
   half_execution_mechanism exec;
   struct _parameter *sploop;
   /*printf("??? %x\n", pc->first_param);
     printf("attempting to liquidate lvalue %x %x\n", pc, pc->first_param->value );*/
   exec = pc->left->exec; /*((struct _execution_pair *)(pc->first_param->value.value))->left;*/
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

void inst_lvalue_call( struct _machine *M )
{
/*
the object on top of the stack had better be a hot call, because we are going to treat it like one.
*/
   int pc = pop( &(M->stack) ).value;
   struct _lvalue *lv = (struct _lvalue *)malloc(sizeof(struct _lvalue) );
   printf("lvalue call\n");
   lv->parameter = pc;
   lv->liquidate = lvalue_call_liquidate;
   push( &(M->stack), object( (int)lv, TYPE_LVALUE ) );
}

void inst_anonymous_parameter( struct _machine *M )
{
   struct _parameter *p = M->next_anonymous_parameter;
   printf("anonymous parameter %x\n", p->value.value);
   M->next_anonymous_parameter = M->next_anonymous_parameter->next;
   push( &(M->stack), p->value );
   printf("fetched anonymous\n");
}

void anonymous_return_liquidate( int address, struct _parameter *rvalues, struct _machine *M )
{
   /*address is presumed to be a stack onto which I will push a return value*/
  push( (struct _stack_member **)address, rvalues->value );
}

void inst_anonymous_return( struct _machine *M )
{
   struct _lvalue *lv = (struct _lvalue *)malloc(sizeof(struct _lvalue) );
   lv->parameter = (int)(&(M->last_anonymous_return));
   lv->liquidate = anonymous_return_liquidate;
   push( &(M->stack), object( (int)lv, TYPE_LVALUE ) );
}

void inst_spawn( struct _machine *M0 )
{
   /*on the stack is a partial call, it will be provided with the downstream bridgehead as its first parameter,
     then loaded into its own machine to execute.*/
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

/*this instruction is at the top of if and do loops to indicate that the value following is just a word of data and
  not an instruction*/
void inst_noop( struct _machine *M )
  {
  M->inst_pointer = M->inst_pointer+1;
  }

/*the tests for if and do seem to be exactly the same*/
void inst_test( struct _machine *M )
  {
  /*in a test, if the value is false (even) we jump forward to the next marker, otherwise we continue*/
  int mark; /*tests include a distance to the next marker*/
  int value = pop(&(M->stack)).value;
  M->inst_pointer = M->inst_pointer+1;
  mark = M->code[M->inst_pointer];
  if(value%2) M->inst_pointer = M->inst_pointer+mark;
  }

void inst_if_mark( struct _machine *M )
  {
  /*at an if marker the machine will get the exit value from the header and jump out*/
  int head; /*markers include a distance back to the header*/
  int end;
  M->inst_pointer = M->inst_pointer+1;
  head = M->code[M->inst_pointer];
  end = M->code[M->inst_pointer - head ];
  M->inst_pointer = M->inst_pointer - head + end;
  }

void inst_do_mark( struct _machine *M )
  {
  /*at a do marker the machine will return to the header*/
  int head; /*markers include a distance back to the header*/
  M->inst_pointer = M->inst_pointer+1;
  head = M->code[M->inst_pointer];
  M->inst_pointer = M->inst_pointer - head + 1;
  }

void inst_left_join( struct _machine *M )
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

int *array_add( int *array, int size, int add_me);
  struct _function_metadata *namespace_join( struct _function_metadata *first, struct _function_metadata *second )
  { 
  struct _function_metadata *m = (struct _function_metadata *)malloc(sizeof(struct _function_metadata));
  int v, i, thing;
  /*to join two functions into the same namespace, the following rules apply
    any variable which :
    is a parameter in the first one is a parameter in the joint
    is a parameter in the second one
       and hasn't been declared in the first one is a parameter in the joint
    is a parameter in the second one 
       and has been declared in the first one
       but is not a parameter in the first one is a local in the joint.
    is local to the first one is local in the joint.
    is local to the second one 
       and is not a parameter in the first one is local in the joint.
    is a return value in both the first one and the second one 
       or a return value in one and not used at all in the other then it is a 
       return value in the joint.
  */
  /*a parameter to the first one is a parameter to the joint*/
  printf("trying to namespace join\n");
  printf("*first %d %x\n", first->num_parameters, first);
  for(i = 0; i < first->num_parameters; i++)
    {
      printf("\t%d", first->parameters[i]);
    }
  printf("\n");
  for(i = 0; i < first->num_locals; i++)
    {
      printf("\t%d", first->locals[i] );
    }
  printf("\n");
  for(i = 0; i < first->num_returns; i++)
    {
      printf("\t%d", first->returns[i] );
    }
  printf("\n*second %d %x\n", second->num_parameters, second); /* second->num_locals, second->num_returns);*/
  for(i = 0; i < second->num_parameters; i++)
    {
      printf("\t%d", second->parameters[i]);
    }
  printf("\n");
  for(i = 0; i < second->num_locals; i++)
    {
      printf("\t%d", second->locals[i] );
    }
  printf("\n");
  for(i = 0; i < second->num_returns; i++)
    {
      printf("\t%d", second->returns[i] );
    }
  printf("\n");
  m->num_parameters = first->num_parameters;
  m->parameters = malloc( sizeof(int) * m->num_parameters ); 
  for( v = 0; v < first->num_parameters; v++ )
     {
     m->parameters[v] = first->parameters[v];
     }
  /*a local to the first one is local to the joint*/
  m->num_locals = first->num_locals;
  m->locals = malloc( sizeof(int) * m->num_locals );
  for( v = 0; v < first->num_locals; v++ )
     {
     m->locals[v] = first->locals[v];
     }
  /*a parameter to the second one that hasn't been declared in the first one is a parameter to the joint*/
  i = 0;
  for( v = 0; v < second->num_parameters; v++ )
     {
     thing = second->parameters[v];
     if(
            !array_contains( first->parameters, thing ) &&
            !array_contains( first->locals, thing ) &&
            !array_contains( first->returns, thing )
          ) 
          { 
	  array_add( m->parameters, m->num_parameters,  thing );
          m->num_parameters = m->num_parameters + 1; 
          i = i + 1;
          }
     }
  /*a local to the second one which is not a parameter to the first one is a local in the joint
    (if it is a local to the first one it has already been declared*/
  i = 0;
  for( v = 0; v < second->num_locals; v++ )
     {
     thing = second->parameters[v];
     if (
        !array_contains( first->parameters, thing) &&
        !array_contains( first->locals, thing ) 
        )
        {
	array_add( m->locals, m->num_locals, thing );
        m->num_locals = m->num_locals + 1;
        i = i+1;
        }
     }
  i = 0;
  /*a return value from the first one which is not a parameter or a local to the second one is a return value from the joint*/
  m->returns =  0;
  m->num_returns = 0;
  for( v = 0; v < first->num_returns; v++ )
     {
     thing = first->returns[v];
     if (
        !array_contains( second->parameters, thing) &&
        !array_contains( second->locals, thing) 
        )
        {
	  array_add( m->returns, m->num_returns, thing );
        m->num_returns = m->num_returns + 1;
        i = i+1;
        } 
     }

  for( v = 0; v < second->num_returns; v++ )
     {
     thing = second->returns[v];
     if (
        !array_contains( first->parameters, thing) &&
        !array_contains( first->locals, thing) &&
        !array_contains( first->returns, thing)
        )
        {
	array_add( m->returns, m->num_returns, thing );
        m->num_returns = m->num_returns + 1;
        i = i+1;
        }
     }
  printf("finished annoying namespace computations\n");
  m->code = copy_function_codeblock( first->code );
  m->exec = namespace_execution_mechanism;
  printf("copy one done\n");
  append_function_codeblock(
                                     m->code,
                                     copy_function_codeblock(second->code)
                                     );
  printf("leaving\n");
  return m;
  }

void inst_namespace_join( struct _machine *M )
{
  struct _partial_call *second = (struct _partial_call *)(pop(&(M->stack)).value);
  struct _partial_call *first = (struct _partial_call *)(pop(&(M->stack)).value); 
  struct _partial_call *ret;
  ret = initialize_partial_call( namespace_join(second->left, first->left), namespace_join(first->right, second->right), 0 );
  push( &(M->stack), object((int)ret, TYPE_SCALAR) );
}

/*end instructions*/
