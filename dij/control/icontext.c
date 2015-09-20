#include "../util/dij_misc.h"
#include "dij_control.h"

/*a context contains enough information for a body of code to be loaded into memory and run*/

/*the context will keep track of a list of codeblocks to be executed in order*/
struct _coderef
   {
   struct _coderef *next, *last;
   struct iCode *payload;
   };

/*this is the result of the fgraph operations, it will contain enough information
to be loaded into a process and executed*/
typedef struct _fcontext
   {
   struct _coderef *head, *tail;
   } *fcontext;

void debug_describe_fcontext( struct iContext *icontext )
   {
   int i;
   struct _coderef *csploop;
   struct _fcontext *context = icontext->C;
   printf("TODO describe context and namespace\n");
//   printf("DESCRIBE FCONTEXT -- %d %d %d %d\n", 
//       context->num_parameters,
//       context->num_locals,
//       context->num_returns,
//       context->num_anonymous);
//   printf("DESCRIBE FCONTEXT -- ");
//   for(i = 0; i < context->num_parameters +
//     context->num_locals +
//     context->num_returns; i++)
//     {
//     printf("%d", context->namespace[i]);
//     }
// 
//   csploop = context->head;
//   while(csploop)
//     {
//     printf("!");
//     debug_describe_icode(csploop->payload);
//     csploop = csploop->next;
//     }
//   printf("\n");
   }




void append_codeblock(struct iContext *self, struct iCode *block)
   {
   struct _fcontext *payload = (struct _fcontext *)(self->C);
   struct _coderef *cc  = (struct _coderef *)malloc(sizeof( struct _coderef)); 
   if(payload->head == 0)
     {
     payload->head = cc;
     }
   if(payload->tail != 0)
     { 
     payload->tail->next = cc;
     cc->last = payload->tail;
     }
   payload->tail = cc;
   cc->payload = block;
   cc->next = 0; 
   }


struct _fcontext * _hack(struct iContext *self)
   {
   return self->C;
   }

/*TODO what if two contexts have different implementations*/
void merge(struct iContext *self, struct iContext *cfirst, struct iContext *csecond)
  {
  int v, i, thing;
  struct _coderef *sploop;
  struct _coderef *copy;
  struct _fcontext *m = (struct _fcontext *)self->C;
  struct _fcontext *first = (struct _fcontext *)cfirst->C;
  struct _fcontext *second = (struct _fcontext *)csecond->C;
  sploop = first->head;
  copy = (struct _coderef *)malloc(sizeof(struct _coderef));
  copy->payload = sploop->payload;
  m->head = copy;
  copy->last = 0;
  while(sploop->next)
     {
     copy->next = (struct _coderef *)malloc(sizeof(struct _coderef));
     copy->next->payload = sploop->next->payload;
     copy->next->last = copy;
     copy = copy->next;
     sploop = sploop->next;
     }
  sploop = second->head;
  copy->next = (struct _coderef *)malloc(sizeof(struct _coderef));
  copy->next->last = copy;
  copy = copy->next;
  copy->payload = sploop->payload;
  while(sploop->next)
     {
     copy->next = (struct _coderef *)malloc(sizeof(struct _coderef));
     copy->next->payload = sploop->next->payload;
     copy->next->last = copy;
     copy = copy->next;
     sploop = sploop->next;
     }
  copy->next = 0;
  m->tail = copy;

  self->i_namespace->merge(self->i_namespace, cfirst->i_namespace, csecond->i_namespace);

  sploop = m->head;
  while( sploop ) { printf("*"); sploop = sploop->next; }
  printf("\n");

   }

/*
struct iContext *copy(struct iContext *self)
  {
  struct iContext *ret = new_context();
  void *sploop;
  struct _fcontext *retc, *selfc;
  retc= (struct _fcontext *)(ret->C);
  selfc = (struct _fcontext *)(self->C);
  *retc = *selfc;
  sploop = 0;
  self->iterate(self, &sploop);
  while( sploop != 0 )
     {
     ret->append_codeblock( ret, self->iterate(self, &sploop) );
     }
  return ret;
  }
*/

void prepend_codeblock(struct iContext *self, struct iCode *block)
   {
   struct _fcontext *payload = (struct _fcontext *)(self->C);
   struct _coderef *cc  = (struct _coderef *)malloc(sizeof( struct _coderef)); 
   cc->next = payload->head;
   payload->head = cc;
   if(payload->tail == 0)
     {
     payload->tail = cc;
     }
   }

//void narrow(struct iContext *self, int num_parameters, int *parameter_indices)
//   {
//   int params_i, locals_i, nsize, totalsize, i, asize, csize;
//   int num_parameters, num_locals, num_returns, num_anonymous;
//   int r_num_parameters, r_num_locals, r_num_returns, r_num_anonymous;
//   struct _parameter *psploop = right.parameters;
//   struct _apply_controller *ac;
//   struct iCode *applyCode;
//   void *sploop;
//   printf("icontext->narrow\n");
//   nsize = 0;
//   asize = 0;
//   csize = 0;
//   left->get_sizes(left, &num_parameters, &num_locals, &num_returns, &num_anonymous);
//   //lasta = left->num_parameters;
//   i = 0;
//   /*this is the size of the left hand side memory space, to which will be added the right hand
//     side parameters*/ 
////   I might not even need it...
////   totalsize = (left->num_parameters+left->num_locals+left->num_returns+left->num_anonymous);
//
//   while( psploop )
//      {
//      if( i < num_parameters && left->namespace[i] != -1 && psploop->curry_flag == 0 )
//         {
//         /*counting named parameters which are populated by this list*/
//         nsize = nsize+1;
//         }
//      if( i >= num_parameters && psploop->curry_flag == 0 )
//         {
//         /*this parameter is passed anonymously, it will go in the anonymous space*/
//         asize = asize+1;
//         }
//      if( i >= num_parameters && psploop->curry_flag == -1 )
//         {
//         /*this parameter is implied by a curry, it will go at the end of the parameter space
//           with a name -1*/
//         csize = csize+1;
//         }
//      i = i+1;
//      psploop = psploop->next;
//      }
//   r_num_parameters = num_parameters - nsize + csize;
//   r_num_locals = num_locals + nsize;
//   r_num_returns = num_returns;
//   r_num_anonymous = num_anonymous + asize;
//   result->size_namespace(result, r_num_parameters,
//                                  r_num_locals,
//                                  r_num_returns,
//                                  r_num_anonymous);
//   ac = new_apply_controller( nsize, asize );
//   
//   psploop = right.parameters;
//   
//   locals_i = result->num_parameters;
//   /*from 0 to left->num_parameters-1 the old namespace contains already declaired parameters
//     their names will either stay in the parameter space or, if they are being populated
//     by the parameter list moved to either the named local variables, or be removed from
//     the namespace to be passed anonymously*/
//   for( i = 0, params_i = 0; i < num_parameters; i++ )
//      {
//      /*if the i'th parameter is curried (or missing), then it remains an unbound parameter*/
//      if( !psploop || psploop->curry_flag == -1 )
//         {
//         result->namespace[params_i] = left->namespace[i]; 
//         params_i = params_i + 1;
//         } else { /*in particular psploop && psploop->curry_flag != -1*/
//         if( left->namespace[i] != -1 )
//            {
//            /*if the parameter has a name, then it becomes a local variable, if it doesn't
//              corrispond to a named parameter, then it will end up in the anonymous memory*/
//            result->namespace[locals_i] = left->namespace[i];
//            locals_i = locals_i + 1;
//
//            add_named_substitution( ac, left->namespace[i], psploop->value );
//
//            } else {
//
//            add_anonymous_substitution( ac, psploop->value );
//
//            }
//         }
//      if( psploop) { psploop = psploop->next; }
//      }
//   /*any space left in the parameter space is implied by curried anonymous parameters
//     it is populated by -1*/
//   for(; i < r_num_parameters ; params_i++, i++ )
//      {
//      result->namespace[params_i] = -1;
//      }
//   /*if we are still not out of parameters passed, they are all anonymous*/
//   while( psploop )
//      {
//      add_anonymous_substitution( ac, psploop->value );
//      psploop = psploop->next;
//      }
//   /*the rest of the namespace is the same in the result as on the left*/
//   for
//      (; 
//      locals_i < r_num_parameters+r_num_locals+r_num_returns; 
//      locals_i++, i++
//      ){
//      result->namespace[locals_i] = left->namespace[i];
//      }
//   /*TODO I should really be keeping track of my iCode elements if I am going to be
//     TODO creating them dynamically */
//
// }

struct iCode *iterate(struct iContext *self, void **token)
   {
   struct _coderef *chain = (struct coderef *)(*token);
   struct _fcontext *C = (struct _fcontext *)(self->C);
   if(chain == 0) {*token = C->head; return 0;} 
   *token = chain->next;
   return chain->payload;
   }

struct iContext *new_context()
   {
   struct iContext *ret;
   struct _fcontext *f = (struct _fcontext *)malloc(sizeof(struct _fcontext));

   f->head = 0;
   f->tail = 0; 

   ret = malloc(sizeof(struct iContext));
   ret->i_namespace = new_namespace();
   ret->C = f;
   ret->append_codeblock = append_codeblock;
   ret->merge = merge;
   ret->iterate = iterate;
   return ret;
   }

