struct _message
   {
   struct _stack_member *message;
   struct _message *next;
   };

struct _channel
   {
   struct _message *first_message;
   struct _message *last_message;
   struct _channel_client *first_client;
   struct _channel_client *last_client;
   };

struct iChannel
   {
   void *C;
   void (*send)(struct iChannel *self, struct _stack_member *data);
   void (*receive)( struct iChannel *self, struct iChannelClient *client);
   };

struct iChannelClient
   {
   void *CC;
   void (*sleep)(iChannelClient *self);
   void (*wake)(iChannelClient *self);
   void (*terminate)(iChannelClient *self);
   };

struct _channel_client
   {
   struct _channel_client *next, *last;
   struct iChannelClient *value;
   };
