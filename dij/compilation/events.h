typedef void (*event_handler)(void *constant_parameter, void *variable_parameter);

struct _event
{
  void *constant_parameter;
  struct _event *next;
  struct _event *last;
  event_handler f;
};

extern struct _event *events_array[255];

void init_events();

void register_event_handler( char event,  event_handler h, void *constant_parameter );

void unregister_event_handler ( char event, event_handler h, void *constant_parameter );

void raise_event( char event, void *variable_parameter);

struct _event *detach_event_handlers( char event, event_handler h, void *constant_parameter );


