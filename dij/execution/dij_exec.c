#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>
#include "dij_misc.h"
#include "../control/dij_control.h"
#include "dij_exec.h"

/*initialization code*/

struct _buffer_chunk
{
  int data[1024];
  int size;
  struct _buffer_chunk *next;
};

struct _buffer
{
   struct _buffer *up;
   struct _buffer_chunk first;
   struct _buffer_chunk *last;
};

struct _buffer *write_buffer = 0;

struct _context_stack
   {
   struct _fcontext *payload;
   struct _context_stack *under;
   int *parameters;
   int *locals;
   int *returns;
   };

struct _context_stack *write_code = 0;

void open_buffer()
{
  struct _buffer *new_buffer;
  new_buffer = malloc(sizeof( struct _buffer ) );
  new_buffer->up = write_buffer;
  write_buffer = new_buffer;
  new_buffer->last = &(new_buffer->first);
  new_buffer->first.size = 0;
  new_buffer->first.next = 0;
}

int *close_buffer()
{
  int *ret;
  int size = 0;
  int w_index = 0;
  int r_index = 0;
  struct _buffer_chunk *sploop = &(write_buffer->first);
  struct _buffer_chunk *kill_me;
  struct _buffer *kill_buffer;
  while( sploop )
  {
    size = size + sploop->size;
    sploop = sploop->next;
  }
  ret = malloc( sizeof(int) * size );
  sploop = &(write_buffer->first);
  while( sploop )
  {
    for( r_index = 0; r_index < sploop->size; r_index++, w_index++ )
    {
       ret[w_index] = sploop->data[r_index];
    }
    sploop = sploop->next;
  }
  sploop = write_buffer->first.next;
  while(sploop) { kill_me = sploop; sploop = sploop->next; free(kill_me); }
  kill_buffer = write_buffer;
  write_buffer = write_buffer->up;
  free(kill_buffer);
  printf("close buffer %x\n", ret);
  return ret;
}

void dij_write( int word )
{
   if( write_buffer == 0 ) { open_buffer(); }
   write_buffer->last->data[write_buffer->last->size] = word;
   write_buffer->last->size++;
   if( write_buffer->last->size == 1024 )
   {
      write_buffer->last->next = malloc(sizeof(struct _buffer_chunk));
      write_buffer->last = write_buffer->last->next;
      write_buffer->last->size = 0;
      write_buffer->last->next = 0;
   }
}

/*this procedure will fail if we are at a 1024 word boundry, but that is okay 
  because it will never be used at 0, and it will always be used in the global
  space, which shouldn't get larger than about 3 words long*/
/*TODO make dij_unwrite less painful in the edge cases that "never" happen*/
int dij_unwrite()
{
  int ret;
  write_buffer->last->size--;
  ret = write_buffer->last->data[ write_buffer->last->size ];
  return ret;
}

void dij_push_data()
{
   open_buffer();
}

void dij_pop_data()
   {
   int *result = close_buffer();
   dij_write( (int)result );
   }

void dij_push_code()
   {
   struct _context_stack *f;
   printf("enter push_code\n");
   open_buffer();
   f = (struct _context_stack *)malloc(sizeof(struct _context_stack));
   f->payload = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   f->payload->num_parameters = 0;
   f->payload->num_locals = 0;
   f->payload->num_returns = 0;
   f->payload->num_anonymous = 0;
   f->parameters = 0;
   f->locals = 0;
   f->returns = 0;
   f->under = write_code;
   write_code = f;
   printf("exit push_code\n");
   }

void dij_pop_code()
   {
   struct _fnode *fnode; 
   struct iCode *icode;
   struct _context_stack *kill_me;
   int i;
   printf("enter pop_code\n");
   write_code->payload->head = malloc(sizeof( struct _coderef)); 
   write_code->payload->head->payload = dij_box( close_buffer() );
   write_code->payload->tail = write_code->payload->head;
   write_code->payload->head->next = 0;
   write_code->payload->namespace = 
      malloc
         (
         sizeof(int)*
            (
            write_code->payload->num_parameters + 
            write_code->payload->num_locals + 
            write_code->payload->num_returns
            )
         );
   printf("1\n");
   for(i = 0; i < write_code->payload->num_parameters; i++) 
      { 
      write_code->payload->namespace[i] = write_code->parameters[i]; 
      }
   for(i = 0; i < write_code->payload->num_locals; i++) 
      { 
      write_code->payload->namespace[i+write_code->payload->num_parameters] =
         write_code->locals[i]; 
      }
   for(i = 0; i < write_code->payload->num_returns; i++) 
      { 
      write_code->payload->namespace
         [i+write_code->payload->num_parameters+write_code->payload->num_locals] = 
         write_code->returns[i]; 
      }
   printf("fgraph->ground %x %x\n", fgraph, fgraph?fgraph->ground:0);
   fnode = fgraph->ground(fgraph, write_code->payload);
   printf("code::%x\n fnode: %x\n", write_code->payload->head, fnode );
   kill_me = write_code;

   write_code = write_code->under;
   if( kill_me )
      {
      if( kill_me->parameters) free(kill_me->parameters);
      if( kill_me->locals) free(kill_me->locals);
      if( kill_me->returns) free(kill_me->returns);
      free(kill_me);
      }
   dij_write( (int) fnode );
   printf("exit pop_code\n");
   }

void dij_define_global()
{
   int number;
   int value;
   printf("define global\n");
   number = dij_unwrite();
   value = dij_unwrite();
   globals[number] = value;
   printf("\t%d %x\n", number, value);
   if( number > max_global ) max_global = number;
}

/*this is a silly way of doing this but I like it much better somehow*/

void dij_note_nothing( int later )
{
}

void dij_note_lvalue( int later )
   {
   /*variable number later is being used as an lvalue in this context, 
     is this the first use? ie is this possibly a return value? if so make 
     sure it has a slot.*/
   if
      ( ! ( 
      array_contains
         (
         write_code->parameters, 
         write_code->payload->num_parameters, 
         later 
         ) || 
      array_contains
         (
         write_code->locals, 
         write_code->payload->num_locals, 
         later
         ) || 
      array_contains
         (
         write_code->returns, 
         write_code->payload->num_returns, 
         later
         ) 
      ) )
      { 
      write_code->returns = 
         array_add(write_code->returns, write_code->payload->num_returns, later);
        write_code->payload->num_returns++;  
      }
   }

void dij_note_rvalue( int later )
   {
   /*variable number later is being used as an rvalue in this context,
     is this the first use? ie is this a parameter? if so make sure it has a slot. 
     otherwise, we now know that it is not a return value, so move it out of that group.*/
   if
      ( ! (
          array_contains(write_code->parameters, write_code->payload->num_parameters, later ) || 
          array_contains(write_code->locals, write_code->payload->num_locals, later ) 
      ) )
      {
      if( array_contains(write_code->returns, write_code->payload->num_returns, later ) )
         {
          write_code->returns = 
             array_remove(write_code->returns, write_code->payload->num_returns, later);
          write_code->locals = array_add(write_code->locals, write_code->payload->num_locals, later);
          write_code->payload->num_returns--;
          write_code->payload->num_locals++;
          } else {
          write_code->parameters = 
             array_add(write_code->parameters, write_code->payload->num_parameters, later);
          write_code->payload->num_parameters++;
          }
       }
   }

void (*number_notes)( int later ) = dij_note_nothing;

void dij_note_number( void (*note_me)( int later ) )
{
   number_notes = note_me; 
}

void dij_record_number( int later )
{
   number_notes(later);
   number_notes = dij_note_nothing;
}

/*end silly thing*/

/*control structures*/
struct _control_structure
{
  int control_type; /*-1 == if ; 0 == do */
  int *control_edit;
  int *marker_edit;
  struct _control_structure *next;
};

struct _control_structure *current_control_structure = 0;

void push_control_structure()
{
  struct _control_structure *ret = malloc(sizeof(struct _control_structure));
  ret->next = current_control_structure;
  current_control_structure = ret;
}

void pop_control_structure()
{
  struct _control_structure *kill_me = current_control_structure;
  current_control_structure = current_control_structure->next;
  free(kill_me);
}

void dij_push_do()
{
  push_control_structure();
  current_control_structure->control_type = 0;
  dij_write((int)inst_noop);
  current_control_structure->control_edit = 
    write_buffer->last->data + write_buffer->last->size;
  dij_write(0);
}

void dij_push_if()
{
  push_control_structure();
  current_control_structure->control_type = -1;
  dij_write((int)inst_noop);
  current_control_structure->control_edit = 
     write_buffer->last->data + write_buffer->last->size;
  dij_write(0);
}

int final_distance(int *target)
   {   
   struct _buffer_chunk *sploop;
   int chunks;
   int offset;
   sploop = &(write_buffer->first);
   while( sploop )
      {
      if( target >= sploop->data && target < sploop->data + 1024 )
         {
         offset = target - sploop->data;
         chunks = 0;
         }
      chunks = chunks+1;
      sploop = sploop->next;
      }
   return (chunks-1)*1024 + write_buffer->last->size - offset;
   }

void dij_pop_control()
   {
   *(current_control_structure->control_edit) = 
      final_distance(current_control_structure->control_edit);
   }

void dij_control_test()
   {
   dij_write((int)inst_test);
   current_control_structure->marker_edit = 
      write_buffer->last->data + write_buffer->last->size;
   dij_write(0);
   }

void dij_control_marker()
   {
   if(current_control_structure->control_type)
      {
      dij_write((int)inst_if_mark);
      } else {
      dij_write((int)inst_do_mark);
      }
   *(current_control_structure->marker_edit) = 
      final_distance(current_control_structure->marker_edit);
   }

/*end control structures*/

/*this is the old c_box implementation see c_machine.c*/
/*
struct _partial_call *c_box( int func_ptr )
{
  struct _function_metadata *m = 
    new_function_metadata( C_execution_mechanism, (void *)func_ptr );
  return initialize_partial_call( m, m, 0 );
}
*/

void *dynamic_load( char *library, char *symbol )
   {
   printf( "!!!!!!!!!!load: %s, %s\n", library, symbol );
   return c_box( dlsym( dlopen(library, 1), symbol ), fgraph );
   }

/*end initialization code*/

void initialize()
   {
   fgraph = fgraph_new();
   globals[0] = (int)c_box( (void *)dynamic_load, fgraph );
   write_code = 0;
   write_buffer = 0;
   }

void yylex();

int main( int argc, char *argv[])
{
   struct iProcess *entry_point;
   initialize();
   yylex();
   /*these tasks will be moved to main*/
   printf("ready\n");
   entry_point = fgraph->load( fgraph, (void *)globals[1], 0, 0 ); 
   entry_point->attach( entry_point ); /*TODO create a reasonable exit channel*/
   process_go();
}
