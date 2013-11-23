/*TODO remove reference to standard libraries*/
#include <stdlib.h>
#include <stdio.h>

#include "../execution/dij_misc.h"
#include "dij_control.h"
#include "apply_icode.h"
#include "spawn_icode.h"

typedef long int DIJ_WORD;

union _foperand
   {
   struct _fcontext *context;
   struct _parameter *parameters;
   };

typedef void (*foperation)
   ( 
   struct _fcontext *left,
   union _foperand right,
   struct _fcontext *result
   );

/*the structure of the _fnode is like so :
  it is connected to two other _fnodes, left and right, which are it's parents.  
  It has two possible values, left_result and right_result because it may
  be called on either the left or the right
  when it is first evaluated on the left or right the result is cached, because
  it is immutable*/
typedef struct _fnode
   {
   struct _fnode *left, *right;
   foperation op;
   union _foperand left_result;
   union _foperand right_result;
   int refcount;
   } *fnode;

typedef struct _fblock
   {
   char used;
   char size;
   struct _fblock *next;
   struct _fnode *first;
   } *fblock;

typedef struct _fgraph
   {
   int size;
   fblock first, last;
   } *fgraph;

struct _fnode *_fgraph_new_node( struct _fgraph *fg )
   {
   /*TODO shouldn't I be tracking these somehow*/
   struct _fnode *ret = malloc(sizeof(struct _fnode));
   ret->refcount = 0;
   ret->left = 0;
   ret->right = 0;
   ret->op = 0;
   ret->left_result.context = 0;
   ret->right_result.context = 0;
   return ret;
   }
/*these are the operations performed on an fnode*/

/*the data for apply contains a list of parameters which are to be bound to the context left.
  the result will be to append at the beginning of the code chain a new iCode reference
  which populates the necessary variables. The right parameter is not used and 
  should be 0.  The new context has fewer parameters and more local variables than the 
  old one.
  */

/*this is the foperation signature for the apply process, the left parameter to represent
  the function, the right parameter is a list of parameters passed.
  The namespace will be size of the left parameter, plus any anonymous parameters in the
  substitution list.  The number of parameters will have been reduced by the number of named
  substitutions, increased by the number of curried anonymous parameters.  The local variables
  will have been increased by the number of named substitutions.  An iCode reference to a 
  substitution machine will be placed at the front of the code list.*/
void foperation_apply
   (
   struct _fcontext *left,
   union _foperand right,
   struct _fcontext *result
   )
   {
   int params_i, locals_i, nsize, totalsize, i, asize, csize;
   struct _parameter *psploop = right.parameters;
   struct _apply_controller *ac;
   struct iCode *applyCode;
   struct _coderef *cr, *sploop;
   printf("foperation_apply\n");
   nsize = 0;
   asize = 0;
   csize = 0;
   //lasta = left->num_parameters;
   i = 0;
   /*this is the size of the left hand side memory space, to which will be added the right hand
     side parameters*/
//   I might not even need it...
//   totalsize = (left->num_parameters+left->num_locals+left->num_returns+left->num_anonymous);

   while( psploop )
      {
      if( i < left->num_parameters && left->namespace[i] != -1 && psploop->curry_flag == 0 )
         {
         /*counting named parameters which are populated by this list*/
         nsize = nsize+1;
         }
      if( i >= left->num_parameters && psploop->curry_flag == 0 )
         {
         /*this parameter is passed anonymously, it will go in the anonymous space*/
         asize = asize+1;
         }
      if( i >= left->num_parameters && psploop->curry_flag == -1 )
         {
         /*this parameter is implied by a curry, it will go at the end of the parameter space
           with a name -1*/
         csize = csize+1;
         }
      i = i+1;
      psploop = psploop->next;
      }
   result->num_parameters = left->num_parameters - nsize + csize;
   result->num_locals = left->num_locals + nsize; 
   result->num_anonymous = left->num_anonymous+asize;
   result->num_returns = left->num_returns;
   result->namespace = 
      malloc
         ( 
         sizeof(DIJ_WORD) *
            (
            result->num_parameters +
            result->num_locals +
            result->num_returns
            )
         );

   ac = new_apply_controller( nsize, asize );
   
   psploop = right.parameters;
   
   locals_i = result->num_parameters;
   /*from 0 to left->num_parameters-1 the old namespace contains already declaired parameters
     their names will either stay in the parameter space or, if they are being populated
     by the parameter list moved to either the named local variables, or be removed from
     the namespace to be passed anonymously*/
   for( i = 0, params_i = 0; i < left->num_parameters; i++ )
      {
      /*if the i'th parameter is curried (or missing), then it remains an unbound parameter*/
      if( !psploop || psploop->curry_flag == -1 )
         {
         result->namespace[params_i] = left->namespace[i]; 
         params_i = params_i + 1;
         } else { /*in particular psploop && psploop->curry_flag != -1*/
         if( left->namespace[i] != -1 )
            {
            /*if the parameter has a name, then it becomes a local variable, if it doesn't
              corrispond to a named parameter, then it will end up in the anonymous memory*/
            result->namespace[locals_i] = left->namespace[i];
            locals_i = locals_i + 1;

            add_named_substitution( ac, left->namespace[i], psploop->value );

            } else {

            add_anonymous_substitution( ac, psploop->value );

            }
         }
      if( psploop) { psploop = psploop->next; }
      }
   /*any space left in the parameter space is implied by curried anonymous parameters
     it is populated by -1*/
   for(; i < result->num_parameters ; params_i++, i++ )
      {
      result->namespace[params_i] = -1;
      }
   /*if we are still not out of parameters passed, they are all anonymous*/
   while( psploop )
      {
      add_anonymous_substitution( ac, psploop->value );
      psploop = psploop->next;
      }
   /*the rest of the namespace is the same in the result as on the left*/
   for
      (; 
      locals_i < result->num_parameters+result->num_locals+result->num_returns; 
      locals_i++, i++
      ){
      result->namespace[locals_i] = left->namespace[i];
      }
   /*TODO I should really be keeping track of my iCode elements if I am going to be
     TODO creating them dynamically */

   applyCode = apply_get_iCode( ac );

   cr = malloc(sizeof(struct _coderef));
   cr->payload = applyCode;
   cr->last = 0;
   cr->next = 0;
   result->head = cr;
   sploop = left->head;
   while( sploop != 0 )
      {
      cr->next = malloc(sizeof(struct _coderef));
      cr->next->payload = sploop->payload;
      cr->next->last = cr;
      cr = cr->next;
      sploop = sploop->next;
      }
   result->tail = cr;	
   cr->next = 0;
   }

union _foperand fnode_eval_recursive( void *fnode, int left )
   {
   struct _fnode *node = (struct _fnode *)fnode;
   union _foperand new_operand;
   printf
      (
      "fgraph_eval_recursive %lx %lx %lx %lx %lx %lx\n", 
      node, 
      node->op,
      node->left_result, 
      node->right_result, 
      node->left, 
      node->right
      );

   if(node->left_result.context && left) 
      {
      printf("return lefthand result\n");
      return node->left_result;
      }
   if(node->right_result.context && !left) 
      {
      printf("return righthand result\n"); 
      return node->right_result;
      }

   if(node->op != 0)
      {
      new_operand.context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
      printf("node op\n");
      node->op
         (
         fnode_eval_recursive( node->left, left ).context,
         fnode_eval_recursive( node->right, left),
         new_operand.context
         );
      if(left) {node->left_result = new_operand;} else {node->right_result = new_operand;}
      return new_operand;
      }
   if(left) 
      { 
      fnode_eval_recursive( node->left, left ); 
      node->left_result = node->left->left_result; 
      return node->left_result;
      }
   if(!left) 
      {
      fnode_eval_recursive( node->right, left );
      node->right_result = node->right->right_result;
      return node->right_result; 
      }
   }

int fgraph_is_loadable( struct iFGraph *self, void *fnode, int left )
   {
   struct _fcontext *context;
   printf("is_loadable\n");
   context = fnode_eval_recursive( (struct _fnode *)fnode, left).context;
   printf("done with is_loadable\n");
   return (context->num_parameters == 0)?-1:0;
   }

struct iProcess *fgraph_load( struct iFGraph *self, void *fnode, struct iChannel *exit_channel, int left )
   {
   struct _fcontext *context;
   struct iProcess *process;
   struct _coderef *sploop;
   struct iMachine *machine;
   struct iCode *code;
   int memory_size;
   printf("fgraph_load\n");
   context = fnode_eval_recursive( (struct _fnode *)fnode, left ).context;
   printf("complete load\n");
   process = new_iProcess();

   if( context->num_parameters != 0 )
      {
      printf("ERROR :: ATTEMPTED TO EXECUTE CODE WITH UNBOUND PARAMETERS\n");
      }

   memory_size = 
      context->num_locals + 
      context->num_returns + 
      context->num_anonymous;

   process->initialize
      ( 
      process, 
      memory_size, 
      context->num_locals, 
      context->num_returns, 
      exit_channel 
      );
   /*
     TODO resolve the questions (with regard to multiple iMachines) about the use of
     TODO anonymous memory and make sure the data structure gets cleaned up
   */
/*this is probably garbage, I won't be needing a distinct anonymous memory class*/
/*
   anonymous_memory = (struct iAnonymousMemory *)malloc(sizeof(struct iAnonymousMemory));
   anonymous_memory->AM = 
      (struct _anonymous_memory *)malloc
         (
         sizeof(struct _anonymous_memory)
         );
   anonymous_memory->AM->p = process;
   anonymous_memory->AM->start_index = context->num_locals + context->num_returns;
   anonymous_memory->AM->end_index = 
      context->num_locals + 
      context->num_returns + 
      context_num_anonymous;
   anonymous_memory->AM->read_offset = 0;
   anonymous_memory->AM->write_offset = 0;
   anonymous_memory->length = process_anonmem_length;
   anonymous_memory->read = process_anonmem_read;
   anonymous_memory->write = process_anonmem_write;
   anonymous_memory->out = process_anonmem_out;
*/

   sploop = context->head;
   while( sploop != 0 )
      {
      code = sploop->payload;
      machine = code->new
         ( 
         code,
         context,
         process,
         self
         );
      process->append(process, machine);
      sploop = sploop->next;
      }
   return process;
   }

void *fgraph_ground
   (
   struct iFGraph *self, 
   struct _fcontext *new_context
   )
   {
   struct _fgraph *fg = (struct _fgraph *)(self->FG);
   struct _fnode *neww; 
   printf("fgraph_ground\n");
   neww = _fgraph_new_node( fg );
   neww->left_result.context = new_context;
   neww->right_result.context = new_context;
   //TODO make sure to null the parent pointers
   return neww;
   }

void *fgraph_apply( struct iFGraph *self, void *fnode, struct _parameter *p )
   {
   struct _fnode *left = (struct _fnode *)fnode;
   struct _fnode *right, *center;
   struct _fgraph *fg = (struct _fgraph *)(self->FG);
   printf("fgraph_apply\n");
   right = _fgraph_new_node( fg );
   right->right_result.parameters = p;
   right->left_result.parameters = p;
   center = _fgraph_new_node( fg );
   center->right = right;
   center->left = left;
   center->op = foperation_apply;
   self->reference(self, left);
   self->reference(self, right);
   return center;
   }

void *fgraph_fcurry( struct iFGraph *self, struct _parameter *p )
   {
   }

void *fgraph_spawn( struct iFGraph *self, void *fnode )
   {
   /*this will create three nodes.  
     Two will be based on channel machines, 
     and one will consitute the function to be run. 
     the function to be run will be constructed as follows :
     there will be one passed parameter, an apply machine will be created for that purpose.
     there will also be a machine which takes all the other variables from the current context
     before losing control.
     The other two will just be grounded.
   */
   struct _fnode *mine = (struct _fnode *)_fgraph_new_node((struct _fgraph *)(self->FG));
   struct _fnode *yours = (struct _fnode *)_fgraph_new_node((struct _fgraph *)(self->FG));;
   struct _parameter *p = (struct _parameter *)malloc(sizeof(struct _parameter));
   struct iChannel *upstream = channel_new();
   struct iChannel *downstream = channel_new();
   struct _fcontext *context;
   struct iProcess *process;
   void *node;

   printf("fgraph_spawn\n");

   mine->left_result.context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   mine->right_result.context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   yours->left_result.context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   yours->right_result.context = (struct _fcontext *)malloc(sizeof(struct _fcontext));
   mine->left = 0;
   mine->right = 0;
   mine->op = 0;
   yours->left = 0;
   yours->right = 0;
   yours->op = 0;

   context = mine->left_result.context; /*on the left, I send downstream*/
   context->num_parameters = 0;
   context->num_locals = 0;
   context->num_returns = 0;
   context->num_anonymous = 0;
   context->namespace = 0;
   context->head = (struct _coderef *)malloc(sizeof(struct _coderef));
   context->tail = context->head;
   context->head->payload = get_channel_send(downstream);
   context->head->next = 0;
   context->head->last = 0;

   context = yours->right_result.context; /*on the right, you receive downstream*/
   context->num_parameters = 0;
   context->num_locals = 0;
   context->num_returns = 0;
   context->num_anonymous = 0;
   context->namespace = 0;
   context->head = (struct _coderef *)malloc(sizeof(struct _coderef));
   context->tail = context->head;
   context->head->payload = get_channel_receive(downstream);
   context->head->next = 0;
   context->head->last = 0;

   context = mine->right_result.context; /*on the right, I receive upstream*/
   context->num_parameters = 0;
   context->num_locals = 0;
   context->num_returns = 0;
   context->num_anonymous = 0;
   context->namespace = 0;
   context->head = (struct _coderef *)malloc(sizeof(struct _coderef));
   context->tail = context->head;
   context->head->payload = get_channel_receive(upstream);
   context->head->next = 0;
   context->head->last = 0;

   context = yours->left_result.context; /*on the left, you send upstream*/
   context->num_parameters = 0;
   context->num_locals = 0;
   context->num_returns = 0;
   context->num_anonymous = 0;
   context->namespace = 0;
   context->head = (struct _coderef *)malloc(sizeof(struct _coderef));
   context->tail = context->head;
   context->head->payload = get_channel_send(upstream);
   context->head->next = 0;
   context->head->last = 0;

   /*I will now start your node, but first I need to apply your node*/
   /*TODO at some point in the process this gets turned into an TYPE_SCALAR in the coroutine unit test*/
   p->value.value = (long int)yours;
   p->value.type = &TYPE_F_NODE;
   p->curry_flag = 0;
   node = fgraph_apply( self, fnode, p);
   process = fgraph_load( self, node, 0, 0 );
   process->attach(process);
   return mine;
   }

void *fgraph_ljoin( struct iFGraph *self, void *fnode_left, void *fnode_right )
   {
   struct _fnode *left = (struct _fnode *)(fnode_left);
   struct _fnode *right = (struct _fnode *)(fnode_right);
   struct _fnode *ret = (struct _fnode *)_fgraph_new_node((struct _fgraph *)(self->FG));
   ret->left_result.context = 0;
   ret->right_result.context = 0;
   ret->left = left;
   ret->right = right;
   ret->op = 0;
   return ret;
   }

void foperation_njoin( struct _fcontext *first, union _foperand second_o, struct _fcontext *m )
  { 
  int v, i, thing;
  int num_parameters, num_locals, num_returns;
  int temp_parameters[256];
  int temp_locals[256];
  int temp_returns[256];
  struct _fcontext *second = second_o.context;
  struct _coderef *sploop;
  struct _coderef *copy;
  /*to join two functions into the same namespace, the following rules apply
    any variable which :
    is a parameter in the first one is a parameter in the joint
    is a parameter in the second one
       and hasn't been declared in the first one is a parameter in the joint
    is a parameter in the second one 
       and has been declared in the first one
       but is not a parameter in the first one is a local in the joint.
    is local to the first one is local in the joint.
    is local to the second one 
       and is not a parameter in the first one is local in the joint.
    is a return value in both the first one and the second one 
       or a return value in one and not used at all in the other then it is a 
       return value in the joint.
  */
 
  /*first step is to find the sizes in the new namespace*/
  /*
    is a parameter in the first one is a parameter in the joint */
  num_parameters = first->num_parameters;
  for( i = 0; i < first->num_parameters; i++ )
     {
     temp_parameters[i] = first->namespace[i];
     }
  /*
    is a parameter in the second one
       and hasn't been declared in the first one is a parameter in the joint */
  for( i = 0; i < second->num_parameters; i++ )
    {
    if( !array_contains( first->namespace, first->num_parameters+first->num_locals+first->num_returns, second->namespace[i] ) ) 
      { 
      temp_parameters[num_parameters] = second->namespace[i];
      num_parameters = num_parameters + 1; 
      }
    }

  /*is local to the first one is local in the joint
    is a parameter in the second one 
       and has been declared in the first one
       but is not a parameter in the first one is a local in the joint.
  */
  num_locals = first->num_locals;
  for( i=0; i < first->num_locals; i++)
     {
     temp_locals[i] = first->namespace[i+num_parameters];
     }
  for( i=0; i < second->num_parameters; i++ )
     {
     if( array_contains( first->namespace+first->num_parameters, first->num_returns+first->num_locals, second->namespace[i+second->num_parameters] ) ) 
       {
       temp_locals[num_locals] = second->namespace[i+second->num_parameters];
       num_locals = num_locals+1;
       }
     }

/*
    is a return value in both the first one and the second one 
       or a return value in one and not used at all in the other then it is a 
       return value in the joint.
  */
  num_returns = first->num_returns;
  for( i=0; i < first->num_returns; i++ )
     {
     temp_returns[i] = first->namespace[i+first->num_parameters+first->num_locals];
     }
  for( i = second->num_parameters+second->num_locals; i < second->num_parameters+second->num_locals+second->num_returns; i++)
    {
    if( !array_contains( first->namespace+first->num_parameters+first->num_locals, first->num_returns, second->namespace[i] ) )
       {
       temp_returns[num_returns] = second->namespace[i];
       num_returns = num_returns + 1;
       }
    }

  printf("*first %d %d %d %x\n", first->num_parameters, first->num_locals, first->num_returns, first);
  for(i = 0; i < first->num_parameters+first->num_locals+first->num_returns; i++)
    {
      printf("\t%d\n", first->namespace[i]);
    }
  printf("\n*second %d %d %d %x\n", second->num_parameters, second->num_locals, second->num_returns, second);

  for(i = 0; i < second->num_parameters+second->num_locals+second->num_returns; i++)
    {
      printf("\t%d\n", second->namespace[i]);
    }
  printf("\n");

  m->namespace = malloc(sizeof(int) * (num_parameters+num_locals+num_returns) );
  m->num_parameters = num_parameters;
  m->num_locals = num_locals;
  m->num_returns = num_returns;
  m->num_anonymous = first->num_anonymous + second->num_anonymous;
  for( i = 0; i < num_parameters; i++ )
     {
     m->namespace[i] = temp_parameters[i];
     }
  for( i = 0; i < num_locals; i++ )
     {
     m->namespace[i+num_parameters] = temp_locals[i];
     }
  for( i = 0; i < num_returns; i++ )
     {
     m->namespace[i+num_parameters+num_locals] = temp_returns[i];
     }

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

  printf("\n*result %d %d %d %x\n", m->num_parameters, m->num_locals, m->num_returns, m);

  for(i = 0; i < m->num_parameters+m->num_locals+m->num_returns; i++)
    {
      printf("\t%d\n", m->namespace[i]);
    }

  sploop = m->head;
  while( sploop ) { printf("*"); sploop = sploop->next; }
  printf("\n");

  }

void *fgraph_njoin( struct iFGraph *self, void *fnode_left, void *fnode_right )
   {
   struct _fnode *left = (struct _fnode *)fnode_left;
   struct _fnode *right = (struct _fnode *)fnode_right;
   struct _fnode *center;
   struct _fgraph *fg = (struct _fgraph *)(self->FG);
   printf("fgraph_njoin\n");
   center = _fgraph_new_node( fg );
   center->right = right;
   center->left = left;
   center->op = foperation_njoin;
   self->reference(self, left);
   self->reference(self, right);
   return center;
   }

void fgraph_reference( struct iFGraph *self, void *fnode ) 
   {
   }

void fgraph_unreference( struct iFGraph *self, void *fnode )
   {
   }

void fgraph_destroy( struct iFGraph *self)
   {
   }

/*a factory for getting a new iFGraph*/
struct iFGraph *fgraph_new()
   {
   struct iFGraph *ret = (struct iFGraph *)malloc(sizeof(struct iFGraph));
   ret->FG = malloc(sizeof(struct _fgraph));
   ret->load = fgraph_load;
   ret->ground = fgraph_ground;
   ret->apply = fgraph_apply;
   ret->fcurry = fgraph_fcurry;
   ret->spawn = fgraph_spawn;
   ret->ljoin = fgraph_ljoin;
   ret->njoin = fgraph_njoin;
   ret->reference = fgraph_reference;
   ret->unreference = fgraph_unreference;
   ret->destroy = fgraph_destroy;
   ret->is_loadable = fgraph_is_loadable;
   printf("made an fgraph %x\n", ret);
   return ret;
   }

/*debugging calls*/
void debug_describe_parameter_list( struct iFGraph *self, void *node )
   {
   struct _fnode *fnode = (struct _fnode *)node;
   struct _parameter *psploop;
   printf("DESCRIBE PARAMS --");
   psploop = fnode->right_result.parameters;
   while(psploop)
     {
     if(psploop->curry_flag)
       {
       printf(".");
       } else {
       printf("X");
       }
     psploop = psploop->next;
     }
   printf("\n");
   }

void debug_describe_fnode( struct iFGraph *self, void *node )
   {
   struct _fnode *fnode = (struct _fnode *)node;
   fnode_eval_recursive(fnode, 0);
   fnode_eval_recursive(fnode, -1);
   int i;
   printf("DESCRIBE FNODE -- {\n");
   if( fnode->right && fnode->op != foperation_apply ) 
     { 
     debug_describe_fnode(self, fnode->right); 
     }
   if( fnode->right && fnode->op == foperation_apply )
     {
     debug_describe_parameter_list(self, fnode->right);
     }
   if( fnode->left ) { debug_describe_fnode(self, fnode->left); }
   if( fnode->op == foperation_apply ) { 
     printf("DESCRIBE FNODE -- apply\n");
    }
   if( fnode->op == foperation_njoin ) { printf("DESCRIBE FNODE -- njoin\n"); }
   if( fnode->left_result.context)
     {
     printf("DESCRIBE FNODE -- %d %d %d %d\n", 
       fnode->left_result.context->num_parameters,
       fnode->left_result.context->num_locals,
       fnode->left_result.context->num_returns,
       fnode->left_result.context->num_anonymous);
     printf("DESCRIBE FNODE -- ");
     for(i = 0; i < fnode->left_result.context->num_parameters +
       fnode->left_result.context->num_locals +
       fnode->left_result.context->num_returns; i++)
       {
       printf("%d", fnode->left_result.context->namespace[i]);
       }
     printf("\n");
     }
   printf("DESCRIBE FNODE -- }\n");
   }

void fgraph_debug( struct iFGraph *self, void *fnode, int option)
   {
   debug_describe_fnode( self, fnode );
   }
