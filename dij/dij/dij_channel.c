struct _machine *first_machine, *last_machine;

void machine_add_after( struct _machine *before, struct _machine *after )
   {
   if( !after ) return;
   after->next = before->next;
   before->next = after;
   after->last = before;
   if(after->next) after->next->last = after;
   printf("\tadd machine %x\n", after );
   }

void machine_remove( struct _machine *remove_me )
   {
   if( remove_me->last ) remove_me->last->next = remove_me->next;
   if( remove_me->next ) remove_me->next->last = remove_me->last;
   remove_me->next = 0;
   remove_me->last = 0;
   printf("\tremove machine %x\n", remove_me );
   }

void channel_transmit( struct _machine *m, struct _stack_member *data )
   {
   struct _stack_member *sploop = data;
   printf("transmiting message : \n");
   while(sploop) 
      {
      printf("\t%x\n", sploop->value.value);
      sploop = sploop->under;
      }
   graft(&(m->stack), data);
   machine_remove( m );
   if( !first_machine ) first_machine = m;
   if( last_machine ) machine_add_after( last_machine, m );
   last_machine = m;
   if(m && m->mode == MODE_WAIT) m->mode = MODE_RUN;
   printf("message complete\n");
   } 

void channel_send( struct _channel *c, struct _stack_member *data )
   {
   struct _machine *M = c->first_client;
   struct _message *m;
   if( M )
      {
      printf("send data ready\n");
      c->first_client = M->next;
      channel_transmit( M, data);
      } else {
      printf("send data not ready\n");
      m = malloc(sizeof( struct _message ) );
      m->message = data;
      m->next = 0;
      if(c->last_message) c->last_message->next = m;
      c->last_message = m;
      }
   }

void channel_receive( struct _channel *c, struct _machine *M )
   {
   struct _message *m = c->first_message;
   printf("receive??\n");
   if( m )
      {
      c->first_message = m->next;
      channel_transmit(M, m->message);
      free(m);
      } else {
      if(M) {
         if( first_machine == M ) first_machine = M->next;
         if( last_machine == M ) last_machine = M->last;
         machine_remove( M );
         }
      if( !(c->first_client) ) c->first_client = M;
      if( c->last_client ) machine_add_after( c->last_client, M );
      c->last_client = M;
      if(M) M->mode = MODE_WAIT;
      }
   printf("motherfucker set to receive\n");
   }
