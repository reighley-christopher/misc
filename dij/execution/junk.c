/*JUNK 1
this material was used to handle lvalue and partial calls before I wrote the iCode interface*/

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

/*end JUNK 1*/