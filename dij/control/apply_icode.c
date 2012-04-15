/*TODO remove reference by standard library*/
#include <stdlib.h>
#include <stdio.h>

#include "../execution/dij_misc.h"
#include "apply_icode.h"
#include "dij_control.h"
/*the apply iMachine implimentation just makes substitutions according to a specific
  list of name/value pairs, the list will be terminated with a name=-1*/

struct _apply_controller *new_apply_controller( int nsize, int asize )
   {
   struct _apply_controller *ret = 
      (struct _apply_controller *)malloc(sizeof(struct _apply_controller) );
   ret->memory = malloc( sizeof( struct _apply_substitution ) * (nsize+asize+2) );
   ret->memory[nsize].name = -1;
   ret->memory[asize+nsize+1].name = -1;
   ret->named_i = 0;
   ret->anon_i = nsize+1;
   ret->nsize = nsize;
   return ret;
   }

/*iCode interface to implement the apply method of the fgraph*/
struct iException *foperation_apply_iMachine_run( struct iMachine *self )
   {
   int index = 0;
   int anon = 0;
   struct _apply_machine *am = 
      (struct _apply_machine *)self->M;
   int *memory = am->memory;
   struct _object_type **types = am->types;
   printf("begin variable substitution\n");
   while( am->substitutions[index].name != -1 )
      {
      memory[am->substitutions[index].name] = am->substitutions[index].value.value;
      types[am->substitutions[index].name] = am->substitutions[index].value.type;
      printf("... %d gets %x(%x)\n", am->substitutions[index].name, am->substitutions[index].value.value, am->substitutions[index].value.type);
      index = index+1;
      }
   printf("finished a variable substitution\n");
   return 0;
   }

void foperation_apply_iMachine_destroy( struct iMachine *self )
   {
   struct _apply_machine *am = (struct _apply_machine *)(self->M);
   free( am->substitutions );
   free( self->M );
   free( self );
   }

struct iMachine *foperation_apply_iCode_new
   ( 
   struct iCode *self,
   fcontext context,
   struct iProcess *process,
   struct iFGraph *fgraph
   )
   {
   struct iMachine *ret;
   struct _apply_substitution *s = 
      (struct _apply_substitution *)self->C;
   struct _apply_machine *m;
   int i, j;
   int anon_ptr = context->num_locals+context->num_returns;
   ret = (struct iMachine *)malloc(sizeof(struct iMachine));
   m = (struct _apply_machine *)malloc(sizeof(struct _apply_machine));
   ret->destroy = foperation_apply_iMachine_destroy;
   ret->run = foperation_apply_iMachine_run;
   process->get_memory( process, &(m->memory), &(m->types), 0 );
   //ret->send = foperation_apply_iMachine_send;
   i = 0;
   while( s[i].name != -1 )
      {
      i = i+1;
      }
   i = i+1;
   while( s[i].name != -1 )
      {
      i = i+1;
      }
   printf("... %d substitutions\n", i);
   m->substitutions = malloc( sizeof(struct _apply_substitution)*i );
 
   ret->M = m;
   i = 0;
   while( s[i].name != -1 )
      {
      j = 0;
      while( context->namespace[j] != s[i].name ) { j = j+1; }
      m->substitutions[i].name = j;
      m->substitutions[i].value = s[i].value;
      printf("%d\n", i);
      i = i+1;
      }
   i = i+1;
   while( s[i].name != -1 )
      {
      m->substitutions[i-1].name = anon_ptr;
      m->substitutions[i-1].value = s[i].value;
      anon_ptr = anon_ptr+1;
      printf("%d\n", i-1);
      i = i+1;
      }
   m->substitutions[i-1].name = -1;
   printf("%d\n", i);
   printf("built substitution map\n");
   return ret;
};

struct iCode *apply_get_iCode( struct _apply_controller *AC )
   {
   struct iCode *ret = (struct iCode *)malloc( sizeof(struct iCode) );
   ret->C = AC->memory;
   ret->new = foperation_apply_iCode_new;
   return ret;
   }

void add_named_substitution( struct _apply_controller *AC, int name, struct _object value )
   {
   AC->memory[AC->named_i].name = name;
   AC->memory[AC->named_i].value = value;
   AC->named_i = AC->named_i+1;
   }

void add_anonymous_substitution( struct _apply_controller *AC, struct _object value )
   {
   AC->memory[AC->anon_i].name = AC->anon_i-AC->nsize-1; 
   AC->memory[AC->anon_i].value = value;
   AC->anon_i = AC->anon_i+1;
   }

struct iCode *new_apply_icode()
   {
   }


/*TODO implement foperation_apply_iMachine_send*/
/*don't think I need this now, machine is to create a channel for this purpose*/
//void foperation_apply_iMachine_send( struct iMachine *self, struct _stack_element *data )
//   {
//   printf("**ERROR :: foperation_apply_iMachine_send NOT IMPLEMENTED\n");
//   }

