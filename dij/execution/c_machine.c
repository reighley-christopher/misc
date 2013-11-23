/*TODO eliminate references to standard libraries*/
#include <stdlib.h>

#include "dij_misc.h"
#include "../control/dij_control.h"
#include "dij_exec.h"

/*iCode implementation for executing functions using a C calling convention*/

/*the function c_box creates an iCode implementation out of a C function*/

typedef long int DIJ_WORD;

struct _cbox_machine
   {
   long int size;
   long int *parameters; 
   void *f;
   struct iChannel *anonymous_out;
   };

void cbox_imachine_destroy(struct iMachine *self)
   {
   free(self);
   }

DIJ_WORD __attribute__((cdecl)) dij_c_call(void *f, long int argc, long int *argv );

struct iException *cbox_imachine_run(struct iMachine *self)
   {
   struct _cbox_machine *cm = (struct _cbox_machine *)(self->M);
   struct _stack_member *ret_m = (struct _stack_member *)malloc(sizeof(struct _stack_member ));
   DIJ_WORD ret;
   ret = dij_c_call( cm->f, cm->size, cm->parameters );
   ret_m->value = object( ret, &TYPE_SCALAR );
   ret_m->under = 0;
   printf("cbox return %x", ret);
   cm->anonymous_out->send( cm->anonymous_out, ret_m );
   return 0;
   }

struct iMachine *cbox_icode_new
   (
   struct iCode *self,
   struct _fcontext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
   )
   {
   struct iMachine *ret;
   struct _cbox_machine *cm;
   long int *memory;
   ret = (struct iMachine *)malloc(sizeof(struct iMachine));
   ret->run = cbox_imachine_run;
   ret->destroy = cbox_imachine_destroy;
   cm = (struct _cbox_machine *)malloc(sizeof(struct _cbox_machine));
   ret->M = cm;
   cm->f = self->C;
   /*SPEC process->get_memory should handle null pointers as parameters in a reasonable way*/
   process->get_memory(process, &memory, 0, &(cm->anonymous_out));
   cm->parameters = 
      memory+
      context->num_parameters + 
      context->num_locals + 
      context->num_returns;
   cm->size = context->num_anonymous * sizeof(DIJ_WORD);
//   cm->anonymous_out = anonymous_out;
   return ret;
   }

/*this is the old implementation of the same functionality, for reference*/
/*
void C_execution_mechanism( struct _function_metadata *meta, struct _parameter *p, struct _machine *M )
{
   int size = 0;
   void *f;
   int *v;
   int i;
   struct _stack_member *ret = malloc(sizeof (struct _stack_member) );
   struct _parameter *sploop = p;
   f = meta->code;
   while( !sploop->curry_flag )
   {
     size = size+sizeof(int);
     sploop = sploop->next;
   }
   sploop = p;
   v = (int *)malloc(size);
   i = 0;
   while( !sploop->curry_flag )
   {
     v[i] = sploop->value.value; 
     sploop = sploop->next;
     i = i+1;
   }
   printf("calling : %d\n", size );
   ret->value.value = dij_c_call(f, size, v);
   ret->value.type = TYPE_SCALAR;
   ret->under = 0;
   free(v);
   if(M) graft( &(M->stack) ,ret);
}
*/

void *c_box( void *c_function, struct iFGraph *fgraph )
   {
   struct iCode *code = (struct iCode *)malloc(sizeof(struct iCode));
   struct _fcontext *context;
   void *fnode;
   printf("started cbox\n");
   code->new = cbox_icode_new;
   code->C = c_function;
   context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   context->head = (struct _coderef *)malloc(sizeof(struct _coderef));
   context->tail = context->head;
   context->head->next = 0;
   context->head->payload = code;
   context->num_parameters = 0;
   context->num_locals = 0;
   context->num_returns = 0;
   fnode = fgraph->ground(fgraph, context);
   printf("finished cbox %x\n", fnode);
   return fnode;
   }

/*end c_box*/

