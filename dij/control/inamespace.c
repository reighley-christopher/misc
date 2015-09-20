#include "../util/dij_misc.h"
#include "dij_control.h"

struct _namespace
  {
  int num_parameters, num_locals, num_returns, num_anonymous;
  int *namespace;
  int *editor_memory;
  int params_i, locals_i, edit_i;
  };

int ns_variable_offset
   ( 
   struct iNamespace *self, 
   int code 
   )
   {
   int i = 0;
   int j = 0;
   struct _namespace *namespace = self->N;
   printf("searching variables: \nparameters\n");
   for( i = 0; i < namespace->num_parameters; i++ )
      {
      printf("%d\n", j);
      if( namespace->namespace[j] == code ) return j;
      j++;
      }
   printf("locals\n");
   for( i = 0; i < namespace->num_locals; i++ )
      {
      printf("%d\n", j);
      if( namespace->namespace[j] == code ) return j;
      j++;
      }
   printf("returns\n");
   for( i = 0; i < namespace->num_returns; i++ )
      {
      printf("%d\n", j);
      if( namespace->namespace[j] == code ) return j;
      j++;
      }
   return -1;
   }

int ns_what_name(struct iNamespace *self, int offset)
   {
   struct _namespace *namespace = self->N;
   return namespace->namespace[offset];
   }

void ns_write_namespace(struct iNamespace *self, int *parameters, int *locals, int *returns)
   {
   struct _namespace *payload = (struct _namespace *)self->N;
   int i;
       
   for(i = 0; i < payload->num_parameters; i++) 
      { 
      payload->namespace[i] = parameters[i]; 
      }
   for(i = 0; i < payload->num_locals; i++) 
      { 
      payload->namespace[i+payload->num_parameters] =
         locals[i]; 
      }
   for(i = 0; i < payload->num_returns; i++) 
      { 
      payload->namespace
         [i+payload->num_parameters+payload->num_locals] = 
         returns[i];
      }
   }

void ns_size_namespace(struct iNamespace *self, int num_parameters, int num_locals, int num_returns, int num_anonymous)
   {
   struct _namespace *N = (struct _namespace *)(self->N);
   N->num_parameters = num_parameters;
   N->num_locals = num_locals;
   N->num_returns = num_returns;
   N->num_anonymous = num_anonymous;
   N->namespace = 
      malloc
         ( 
         sizeof(long int)*
            (
            N->num_parameters + 
            N->num_locals + 
            N->num_returns
            )
         );
   }

int ns_get_size(struct iNamespace *self)
   {
   struct _namespace *N = (struct _namespace *)(self->N);
   return N->num_parameters + N->num_locals + N->num_returns + N->num_anonymous; 
   }

void ns_get_sizes(struct iNamespace *self, int *num_parameters, int *num_locals, int *num_returns, int *num_anonymous)
  {
  struct _namespace *N = (struct _namespace *)(self->N);
  if(num_parameters){*num_parameters = N->num_parameters;}
  if(num_locals){*num_locals = N->num_locals;}
  if(num_returns){*num_returns = N->num_returns;}
  if(num_anonymous){*num_anonymous = N->num_anonymous;}
  }

void ns_merge(struct iNamespace *self, struct iNamespace *cfirst, struct iNamespace *csecond)
  {
  int num_parameters, num_locals, num_returns;
  int i;
  struct _namespace *m = (struct _namespace *)self->N;
  struct _namespace *first = (struct _namespace *)cfirst->N;
  struct _namespace *second = (struct _namespace *)csecond->N;
  int temp_parameters[256];
  int temp_locals[256];
  int temp_returns[256];
 
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

 /*  is a parameter in the first one is a parameter in the joint */


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
  num_returns = 0;
  for( i=0; i < first->num_returns; i++ )
     {
     if( !array_contains( second->namespace, second->num_parameters+second->num_locals, 
                          first->namespace[i+first->num_parameters+first->num_locals]))
       {
       temp_returns[num_returns] = first->namespace[i+first->num_parameters+first->num_locals];
       num_returns = num_returns+1;
       } 
    }
  for( i = second->num_parameters+second->num_locals; i < second->num_parameters+second->num_locals+second->num_returns; i++)
    {
  /*  if( !array_contains( first->namespace+first->num_parameters+first->num_locals, first->num_returns, second->namespace[i] ) )*/
/*TODO: does everything work?  I think this line was a bug!*/
      if( !array_contains( first->namespace, first->num_parameters+first->num_locals+first->num_returns, second->namespace[i] ))
        {
        temp_returns[num_returns] = second->namespace[i];
        num_returns = num_returns + 1;
        }
    }

/*
    is local in the second one and is not a parameter in the first one is local in the joint
*/
  for(i = 0; i < second->num_locals; i++)
    {
    if( !array_contains( first->namespace, first->num_parameters, second->namespace[second->num_parameters+i] ))
      {
      temp_locals[num_locals] = second->namespace[second->num_parameters+i];
      num_locals = num_locals + 1;
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

  printf("\n*result %d %d %d %x\n", m->num_parameters, m->num_locals, m->num_returns, m);
  for(i = 0; i < m->num_parameters+m->num_locals+m->num_returns; i++)
    {
      printf("\t%d\n", m->namespace[i]);
    }

  }

void ns_open(struct iNamespace *self, struct iNamespace *source)
  {
  struct _namespace *n = self->N;
  struct _namespace *s = source->N;
  n->editor_memory = s->namespace;
  n->params_i = 0;
  n->locals_i = n->num_parameters; 
  n->edit_i = 0;
  }

void ns_close(struct iNamespace *self)
  {
  struct _namespace *n = self->N;
  /*any space left in the parameter space is implied by curried anonymous parameters
    it is populated by -1*/
   for(; n->edit_i < n->num_parameters ; n->params_i++, n->edit_i++ )
      {
      n->namespace[n->params_i] = -1;
      }
   /*the rest of the namespace is the same in the result as on the left*/
   for
      (; 
      n->locals_i < n->num_parameters+n->num_locals+n->num_returns; 
      n->locals_i++, n->edit_i++
      ){
      n->namespace[n->locals_i] = n->editor_memory[n->edit_i];
      }
  }

int ns_bind(struct iNamespace *self)
  {
  struct _namespace *n = self->N;
  int ret = n->editor_memory[n->edit_i];
  if(ret != -1)
    {
    n->namespace[n->locals_i] = ret;
    n->locals_i = n->locals_i + 1;
    }
  n->edit_i = n->edit_i + 1;
  return ret;
  }

void ns_curry(struct iNamespace *self)
  {
  struct _namespace *n = self->N;
  n->namespace[n->params_i] = n->editor_memory[n->edit_i]; 
  n->params_i = n->params_i + 1;
  n->edit_i = n->edit_i + 1;
  }

struct iNamespace *new_namespace()
  {
  struct iNamespace *ret;
  ret = (struct iNamespace *)malloc(sizeof(struct iNamespace));
  ret->N = (struct _namespace *)malloc(sizeof( struct _namespace));
  ret->variable_offset = ns_variable_offset;
  ret->what_name = ns_what_name;
  ret->get_sizes = ns_get_sizes;
  ret->get_size = ns_get_size;
  ret->open = ns_open;
  ret->close = ns_close;
  ret->bind = ns_bind;
  ret->curry = ns_curry;
  ret->size_namespace = ns_size_namespace;
  ret->write_namespace = ns_write_namespace;
  ret->merge = ns_merge;
  return ret;
  }
