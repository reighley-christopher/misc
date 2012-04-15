#include <stdlib.h>
#include "events.h"

struct _event *events_array[255];

void init_events()
{
   int i = 0;
   while( i <= 255 ) { events_array[i] = 0; i++; }
}

void register_event_handler( char event,  event_handler h, void *constant_parameter )
{
   struct _event *e;
   e = malloc(sizeof(struct _event));
   e->next = events_array[event];
   if( e->next ) { e->next->last = e; }
   e->last = 0;
   e->f = h;
   e->constant_parameter = constant_parameter;
   events_array[event] = e;
}

void raise_event( char event, void *variable_parameter)
{
   struct _event *e;
   e = events_array[event];
   while( e )
      {
      e->f( e->constant_parameter, variable_parameter );
      e = e->next;
      }
}

void unregister_event_handler ( char event, event_handler h, void *constant_parameter )
{
  struct _event *leap, *frog;
  leap = detach_event_handlers( event, h, constant_parameter );
  while( leap )
  {
     frog = leap->next;
     free(leap);
     leap = frog;
  }
}

struct _event *detach_event_handlers( char event, event_handler h, void *constant_parameter )
{
   struct _event *e;
   struct _event *temp;
   struct _event *ret = 0;
   e = events_array[event];
   while( e )
      {
      if( e->f == h && e->constant_parameter == constant_parameter )
         {
         if( e->last == 0 ) { events_array[event] = e->next; } else { e->last->next = e->next; }
         temp = e;
         if( e->next ) { e->next->last = e->last; }
         e = e->next;
         temp->next = ret;
         temp->last = 0;
         if( ret ) { ret->last = temp; }
         } else {
         e = e->next;
         }
      }
   return ret;
}

void reattach_event_handlers( char event, struct _event *e )
{
   struct _event *end;
   if (!e) return;
   end = e;
   while( end->next ) { end = end->next; }
   end->next = events_array[event];
   events_array[event] = e;
}
