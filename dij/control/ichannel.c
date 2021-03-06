/*TODO remove references to standard libraries*/
#include <stdio.h>
#include <stdlib.h>

/*implementation of a simple channel for communication between processes*/
#include "../util/dij_misc.h"
#include "dij_control.h"

struct _message
   {
   struct _stack_member *message;
   struct _message *next;
   };

struct _channel_client
   {
   struct _channel_client *next;
   struct iChannelClient *payload;
   };

struct _channel
   {
   struct _message *first_message;
   struct _message *last_message;
   struct _channel_client *first_client;
   struct _channel_client *last_client;
   };

void channel_send( struct iChannel *self, struct _stack_member *data )
   {
   /*SPEC channels need to starve out cleanly. They can't have bad pointers
     SPEC to old clients hanging around in channel->last_client*/
   struct _channel *c = (struct _channel *)self->C;
   struct _channel_client *cc = c->first_client;
   struct _message *m;
   if( cc )
      {
      printf("send data ready\n");
      if(c->last_client == cc)
         {
         c->last_client = 0;
         c->first_client = 0;
         } else {
         c->first_client = cc->next;
         }
      free(cc);
      cc->payload->wake( cc->payload, data );
      } else {
      printf("send data not ready\n");
      m = malloc(sizeof( struct _message ) );
      m->message = data;
      m->next = 0;
      if(!(c->first_message)) c->first_message = m;
      if(c->last_message) c->last_message->next = m;
      c->last_message = m;
      }
   return;
   }

void channel_recieve( struct iChannel *self, struct iChannelClient *M )
   {
   struct _channel *c = (struct _channel *)self->C;
   struct _message *m = c->first_message;
   struct _channel_client *cc;
   printf("receive??\n");
   if( m )
      {
      if(c->last_message == m)
         {
         c->last_message = 0;
         c->first_message = 0;
         } else {
         c->first_message = m->next;
         }
      M->wake( M, m->message );
      free(m);
      } else {
      M->sleep(M);
      cc = (struct _channel_client *)malloc( sizeof( struct _channel_client ) );
      cc->payload = M;
      cc->next = 0;
      if( !(c->first_client) ) c->first_client = cc;   
      if( c->last_client ) c->last_client->next = cc;
      c->last_client = cc;
      }
   printf("motherfucker set to receive\n");
   return;
   }

void channel_abandon( struct iChannel *self, struct iChannelClient *client )
   {
   /*
    TODO implement channel_abandon
    search the list of _channelclient structures, for client, then remove it
   */
   }

void channel_close( struct iChannel *self )
   {
   /*
     TODO implement channel_close
     free all the pending messages, terminate all the pending machines (and free the list)
     then free the _channel structure and the iChannel structure*/
   }

struct iChannel *channel_new()
   {
   struct iChannel *ret = (struct iChannel *)malloc(sizeof(struct iChannel));
   struct _channel *c = (struct _channel *)malloc(sizeof(struct _channel));
   c->first_message = 0;
   c->last_message = 0;
   c->first_client = 0;
   c->last_client = 0;
   ret->C = c;
   ret->send = channel_send;
   ret->receive = channel_recieve;
   ret->abandon = channel_abandon;
   ret->close = channel_close;
   return ret;
   }

/*this module also includes two iCode interfaces for sending and recieving on channels*/

/*this machine has no return values, anything passed to it is formed into a message and
  sent on the channel*/
struct iException *send_code_run( struct iMachine *self )
   {
   }

/*this machine has no parameters, anything passed to it is ignored, it calls the recieve
  method of the channel structure, using a channelclient structure that must somehow
  suspend its own process.  When the message is recieved, it is used to populate the
  anonymous returns*/
struct iException *recieve_code_run( struct iMachine *self )
   {
   }

struct iMachine *send_code_new
   ( 
   struct iCode *self,
   struct iContext *context,
   int *memory,
   struct _object_type *types,
   struct iFGraph *fgraph 
   )
   {
   }

/*self->C will contain the channel to be broadcast on.  Somehow an iChannelClient structure
  needs to be generated which will suspend the process.  Control flow must happen like so
  */
struct iMachine *recieve_code_new
   ( 
   struct iCode *self,
   struct iContext *context,
   int *memory,
   struct _object_type *types,
   struct iFGraph *fgraph
   )
   {
   }
