#include <stdio.h>
#include <stdlib.h>
//TODO remove references to standard libraries.

#include "../execution/dij_misc.h"
#include "dij_control.h"

/*implementation of the iprocess interface including the main loop process_go*/

/*a process consists of a series of machines which are to be run, one after the other using the same memory space.*/
struct _process
   {
   int *memory;
   struct _object_type **types;
   int return_offset, return_size;
   struct _machine *head, *tail;
   struct _process *next, *last;
   struct iChannel buffer_channel;
   struct iChannel *exit_channel;
   };

struct _machine
   {
   struct iMachine *payload;
   struct _machine *next;
   /*TODO these two members might never be used*/
   struct _machine *last;
   struct _process *parent;
   };

   void process_send(struct iChannel *self, struct _stack_member *data)
      {
      struct _stack_member *list = (struct _stack_member *)(self->C);
      struct _stack_member *sploop = list;
      if(!sploop)
         {
         self->C = data;
         return;
         }
      while(sploop->under) { sploop = sploop->under; }
      sploop->under = data;   
      return;
      }

   void process_receive( struct iChannel *self, struct iChannelClient *client)
      {
      struct _stack_member *list = (struct _stack_member *)(self->C);
      self->C = 0;
      client->wake(client, list);     
      }

   void process_abandon( struct iChannel *self, struct iChannelClient *client)
      {
      /*this can do nothing, as we aren't keeping track of our listeners just servicing them*/
      }

   void process_close( struct iChannel *self )
      {
      /*TODO the semantics of this are undefined, 
      /*TODO perhaps both close and abandon should generate errors*/
      }

/*
struct _anonymous_memory
   {
   struct _process *p;
   int read_offset;
   int write_offset;
   int start_index;
   int end_index;
   }

struct iAnonymousMemory
   {
   void *AM;
   int (*length)(struct iAnonymousMemory *self);
   struct _object (*read)(struct iAnonymousMemory *self);
   void (*write)(struct iAnonymousMemory *self, struct _object data);
   void (*out)(struct iAnonymousMemory *self, struct _object data);
   }


int process_anonmem_lenth(iAnonymousMemory *self)
   {
   struct _anonymous_memory *am = (struct _anonymous_memory *)(self->AM);
   return am->end_index - am->start_index;
   }

struct _object process_anonmem_read(void *self)
   {
   struct _anonymous_memory *am = (struct _anonymous_memory *)(self->AM);
   struct _object ret;
   ret.type = am->p->types[am->start_index+am->read_offset];
   ret.value = am->p->memory[am->start_index+am->read_offset];
   am->read_offset = am->read_offset+1;
   return ret;
   }

void process_anonmem_write(void *self, struct _object data)
   {
   struct _anonymous_memory *am = (struct _anonymous_memory *)(self->AM);
   am->p->types[am->start_index+am->write_offset] = data.type;
   am->p->memory[am->start_index+am->write_offset] = data.value;
   am->write_offset = am->write_offset+1;
   }

void process_anonmem_out(void *self, struct _object data)
   {
   struct _anonymous_memory *am = (struct _anonymous_memory *)(self->AM);
   struct _stack_element *output = 
      (struct _stack_element *)malloc(sizeof(struct _stack_element));
   struct _stack_element *sploop;
   output->value = data;
   output->under = 0;
   sploop = p->anonymous_returns->message;
   if(!sploop){p->anonymous_returns->message = output; return;}
   while(sploop->under) {sploop = sploop->under;}
   sploop->under = output;
   }
*/

/*this is the implementation of the iProcess interface*/

/*the system has multiple processes running*/
struct _process *first_process, *last_process;

/*this is an implementation of the channel client abstraction for the iprocess,
  originally from dij_channel.c via ichannel.c*/

/*these two methods should be useless, as the machine list has been broken into parts

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
*/
/*
void process_channel_wake( struct iChannelClient *self, struct _stack_member *data )
   {
   struct _process *m = (struct _process *)self->CC;
   struct _stack_member *sploop = data;
   printf("transmiting message : \n");
   while(sploop) 
      {
      printf("\t%x\n", sploop->value.value);
      sploop = sploop->under;
      }
   /*TODO replace this statement with a call to the front machines iChannel*/
/*
   graft(&(m->stack), data);
   machine_remove( m );
   if( !first_process ) first_process = m;
   if( last_process ) last_process->next = m;
   last_process = m;
   printf("message complete\n");
   }

void process_channel_sleep( struct iChannelClient *self )
   {
   struct _process *M = (struct _machine *)self->CC;
   if(M) {
         if( first_process == M ) first_process = M->next;
         if( last_process == M ) last_process = M->last;
         machine_remove( M );
         }
   }
*/


void exit_proxy_send(struct iChannel *self, struct _stack_member *data)
   {
   struct iChannel *e = (struct iChannel *)(self->C);
   if(e) e->send(e, data);
   }

void exit_proxy_recieve( struct iChannel *self, struct iChannelClient *client)
   {
   struct iChannel *e = (struct iChannel *)(self->C);
   if(e) e->receive(e, client);
   }

void exit_proxy_abandon( struct iChannel *self, struct iChannelClient *client)
   {
   struct iChannel *e = (struct iChannel *)(self->C);
   if(e) e->abandon(e, client);
   }

void exit_proxy_close( struct iChannel *self )
   {
   struct iChannel *e = (struct iChannel *)(self->C);
   if(e) e->close(e);
   }

void process_initialize
   (
   struct iProcess *self, 
   int memory_size, 
   int return_offset, 
   int return_size,
   struct iChannel *exit_channel
   )
   {
   struct _process *p = (struct _process *)(self->P);
   p->memory = (int *)malloc(sizeof(int)*memory_size);
   p->types = (struct _object_type **)malloc(sizeof(struct _type *)*memory_size);
   p->return_offset = return_offset;
   p->return_size = return_size;
   p->exit_channel = exit_channel;
   p->head = 0;
   p->tail = 0;
   }

void process_append( struct iProcess *self, struct iMachine *imachine )
   {
   struct _process *process = (struct _process *)(self->P);
   struct _machine *machine = (struct _machine *)malloc(sizeof(struct _machine) );
   machine->payload = imachine;
   machine->parent = process;
   machine->last = process->tail;
   process->tail = machine;
   machine->next = 0;
   if( !process->head )
      {
      process->head = machine;
      } else {
      machine->last->next = machine;
      }
   }

void process_attach(struct iProcess *self)
   {
   struct _process *p = (struct _process *)(self->P);
   if(!first_process) { first_process = p; p->last = 0; }
   if(last_process) { last_process->next = p; p->last = last_process; }
   last_process = p;
   p->next = 0;
   }

void process_detach(struct iProcess *self)
   {
   struct _process *p = (struct _process *)(self->P);
   if(p == first_process) { first_process = p->next; }
   if(p == last_process) { last_process = p->last; }
   if(p->next) { p->next->last = p->last; }
   if(p->last) { p->last->next = p->next; }
   p->next = 0;
   p->last = 0;
   }

void process_get_memory(struct iProcess *self, int **memory, struct _object_type ***types, struct iChannel **anonymous_out)
   {
   struct _process *p = (struct _process *)(self->P);
   if(memory != 0) { *memory = p->memory; }
   if(types != 0) { *types = p->types; }
   if(anonymous_out != 0) { *anonymous_out = &(p->buffer_channel); }
   }

struct iProcess *new_iProcess()
   {
   struct iProcess *ret;
   struct _process *p;
   ret = (struct iProcess *)malloc(sizeof(struct iProcess) );
   p = malloc( sizeof(struct _process) );
   p->buffer_channel.C = 0;
   p->buffer_channel.send = process_send;
   p->buffer_channel.receive = process_receive;
   p->buffer_channel.abandon = process_abandon;
   p->buffer_channel.close = process_close;
   ret->P = p;
   p->next = 0;
   p->last = 0;

/*
   if( !last_process )
      {
      ((struct _process *)(ret->P))->next = 0; 
      ((struct _process *)(ret->P))->last = 0; 
      first_process = ret->P; 
      last_process = ret->P; 
      } else {
      ((struct _process *)(ret->P))->last = last_process;
      last_process->next = ret->P;
      ((struct _process *)(ret->P))->next = 0;
      }
*/
   ret->initialize = process_initialize;
   ret->append = process_append;
   ret->attach = process_attach;
   ret->detach = process_detach;
   ret->get_memory = process_get_memory;
   }

/*run_process will run the first machine on its process list until it halts,
  whereupon it will either clean it up and place itself on the back of the
  process list, or it will print an error message, or it will do nothing*/
void run_process( struct _process *p )
   {
   struct _machine *m;
   struct iMachine *M;
   struct iException *result;
   struct _stack_member *return_message;
   struct _stack_member *sploop;
   int i;
   M = p->head->payload;
   result = M->run(M);
   if(result == 0 ) /*this machine is finished*/
      {
      M->destroy(M);
      m = p->head;
      if(p->head) /*clear it (this conditional should be redundant TODO error on redundancy*/
         {
         p->head = p->head->next;
         free(m);
         }
      }
   if(result == 0 && p->head ) 
      /*there is a new machine yet to be run, reattach process*/
      /*TODO shouldn't I be using attach?*/
      {
      if(!first_process) first_process = p;
      if(last_process) { last_process->next = p; }
      p->last = last_process;
      p->next = 0;
      last_process = p;
      }
   if((int)result != -1 && result != 0) /*error case*/
      {
      printf("**EXCEPTION :: %s\n", result->message(result) );      
      }
   if(p->head == 0)
      {
      /*this process is done, time to kill it and send its memory down the exit channel*/
      sploop = (struct _stack_member *)p->buffer_channel.C;
      return_message = (struct _stack_member *)p->buffer_channel.C;;
      for(i=p->return_offset+p->return_size-1;i >= p->return_offset;i--)
         {
         return_message = (struct _stack_member *)malloc(sizeof(struct _stack_member));
         return_message->under = sploop;
         sploop = return_message;
         return_message->value.value = p->memory[i];
         return_message->value.type = p->types[i];
         }

      if(p->exit_channel) 
         {
         p->exit_channel->send(p->exit_channel, return_message);
         } 
      /*TODO clean up an iChannel when it will no longer be used*/
      /*TODO clean up a lot of things about this process*/
      free(p->memory);
      free(p->types);
      }
   }

void process_go()
   {
   struct _process *current;
   struct _process *sploop;
   printf("ready to begin\n");
   while( first_process )
      {
      current = first_process;
      first_process = first_process->next;
      if( first_process) first_process->last = 0;
      if( last_process == current ) { last_process = 0; } 
      run_process( current );
      printf("context switch\n");
      sploop = first_process;
      while( sploop )
         {
         printf("\tactive process list: %x\n", sploop );
         if( sploop == last_process ) printf("last process %x\n", sploop->next);
         if( sploop == first_process ) printf("first process %x\n", sploop->next);
         sploop = sploop->next;
         }
      /*if(current->mode == MODE_HALT ) destroy_machine(current);*/
      }
   printf("machine has halted normally\n");
   }


