/*TODO remove references to standard libraries*/
#include <stdio.h>
#include <stdlib.h>
/*iCode interface to implement the spawn method of the fgraph*/
#include "../execution/dij_misc.h"
#include "dij_control.h"

struct _channel_send_machine
   {
   struct iProcess *process;
   struct _fcontext *context;
   struct iChannel *channel;
   };

struct _channel_receive_machine
   {
   int kill_flag;
   struct iProcess *process;
   struct _fcontext *context;
   struct iChannel *channel;
   struct iChannelClient *client;
   };

void process_client_sleep(struct iChannelClient *self)
   {
   struct iProcess *p = (struct iProcess *)(self->CC);
   p->detach(p);
   }

void process_client_wake(struct iChannelClient *self, struct _stack_member *data)
   {
   struct iProcess *p = (struct iProcess *)(self->CC);
   struct iChannel *r;
   printf("process_client_wake\n");
   p->get_memory(p, 0, 0, &r);
   r->send(r, data);
   p->attach(p);
   }

void process_client_terminate(struct iChannelClient *self)
   {
   /*TODO how to clean up the process which has died and will never reattach?*/
   struct iProcess *p = (struct iProcess *)(self->CC);
   p->detach(p);
   }

struct iChannelClient *get_process_client(struct iProcess *process)
   {
   struct iChannelClient *ret = (struct iChannelClient *)malloc(sizeof(struct iChannelClient));
   ret->CC = process;
   ret->sleep = process_client_sleep;
   ret->wake = process_client_wake;
   ret->terminate = process_client_terminate;
   return ret;
   }

void channel_receive_iMachine_destroy(struct iMachine *self)
   {
   struct _channel_receive_machine *M = (struct _channel_receive_machine *)(self->M);
   M->channel->abandon(M->channel, M->client);
   free(self->M);
   free(self);
   }

struct iException *channel_receive_iMachine_run(struct iMachine *self)
   {
   struct _channel_receive_machine *M = (struct _channel_receive_machine *)(self->M);
   printf("channel_receive_iMachine_run\n");
   /*TODO I don't like that I had to do this, perhaps the machine should flag itself for removal
     a different way.*/
   if(M->kill_flag)
      {
      printf("...I'm done\n");
      M->channel->abandon(M->channel, M->client);
      return 0;
      } else {
      M->channel->receive( M->channel, M->client);
      M->kill_flag = -1;
      return (struct iException *)(-1);
      }
   }

void channel_send_iMachine_destroy(struct iMachine *self)
   {
   free(self->M);
   free(self);
   }

struct iException *channel_send_iMachine_run(struct iMachine *self)
   {
   /*gathers up all the anonymous parameters and sends them on the channel*/
   struct _channel_send_machine *M = (struct _channel_send_machine *)(self->M);
   int *memory;
   int i;
   struct _object_type **types;
   struct _stack_member *message, *sploop, *sploop_last;
   struct _fcontext *context;
   printf("channel_send_iMachine_run\n");
   M->process->get_memory(M->process, &memory, &types, 0 );
   context = M->context;
   message = 0;
   i = context->num_parameters + context->num_locals + context->num_returns;
   while
      ( 
      i < 
         context->num_parameters + 
         context->num_locals +
         context->num_returns +
         context->num_anonymous 
      )
      {
      sploop = (struct _stack_member *)malloc(sizeof(struct _stack_member));
      sploop->value.value = memory[i];
      sploop->value.type = types[i];
      sploop->under = 0;
      i++;
      if( message == 0 ) { message = sploop; sploop_last = message; } 
      else { sploop_last->under = sploop; }
      }
   M->channel->send(M->channel, message);
   /*TODO who cleans up message here?*/
   return 0;
   }

struct iMachine *channel_send_iCode_new
   ( 
   struct iCode *self,
   struct _fcontext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
   )
   {
   struct iMachine *ret = (struct iMachine *)malloc(sizeof(struct iMachine *));
   struct _channel_send_machine *M 
      = (struct _channel_send_machine *)malloc(sizeof(struct _channel_send_machine));
   M->channel = (struct iChannel *)(self->C);
   M->context = context;
   M->process = process;
   ret->M = M;
   ret->run = channel_send_iMachine_run;
   ret->destroy = channel_send_iMachine_destroy;
   return ret;
   }

struct iMachine *channel_receive_iCode_new
   ( 
   struct iCode *self,
   struct _fcontext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
   )
   {
   struct iMachine *ret = (struct iMachine *)malloc(sizeof(struct iMachine *));
   struct _channel_receive_machine *M
      = (struct _channel_receive_machine *)malloc(sizeof(struct _channel_receive_machine));
   M->process = process;
   M->context = context;
   M->channel = self->C;
   M->client = get_process_client(process);
   M->kill_flag = 0;
   ret->M = M;
   ret->run = channel_receive_iMachine_run;
   ret->destroy = channel_receive_iMachine_destroy;
   return ret;
   }

struct iCode *get_channel_send( struct iChannel *c )
   {
   struct iCode *ret = (struct iCode *)malloc(sizeof(struct iCode));
   ret->C = c;
   ret->new = channel_send_iCode_new;
   return ret;
   }

struct iCode *get_channel_receive( struct iChannel *c )
   {
   struct iCode *ret = (struct iCode *)malloc(sizeof(struct iCode));
   ret->C = c;
   ret->new = channel_receive_iCode_new;
   return ret;
   }
