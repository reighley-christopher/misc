#include "../../util/dij_misc.h"
#include "../../control/dij_control.h"

/*some dummy instantiations to satisfy the compiler*/
/*TODO why is this necessary? some symbols are leaking*/
struct _object_type
   TYPE_SCALAR,
   TYPE_HOT_CALL,
   TYPE_LVALUE,
   TYPE_F_NODE,
   TYPE_SYMBOL,
   TYPE_EMPTY;

struct iMachine *dij_icode_new(
   struct iCode *self,
   struct iContext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
) {
return (struct iMachine *)0;
}

struct iMachine *cbox_icode_new(
   struct iCode *self,
   struct iContext *context,
   struct iProcess *process,
   struct iFGraph *fgraph
)
{
return (struct iMachine *)0;
}
/*end dummy instanciations*/

/*dummy implementations of interfaces to be used in testing*/
/*TODO these might be useful for other purposes too?*/

/*in the simplest case the iMachine and the iCode can share
  an underlying datastructure*/
struct test_M_and_C
  {
  void (*f)(
    struct iContext *cntx, 
    struct iProcess *process, 
    struct iFGraph *fgraph
    );
  struct iContext *cntx;
  struct iProcess *process;
  struct iFGraph *fgraph;
  };

struct iException *test_imachine_run(struct iMachine *self)
  {
  struct test_M_and_C *M = (struct test_M_and_C *)(self->M);
  M->f(M->cntx, M->process, M->fgraph);
  return 0;
  };

void test_imachine_destroy(struct iMachine *self)
  {
  };

struct iMachine *test_icode_new
  (
  struct iCode *self,
  struct _fcontext *context,
  struct iProcess *process,
  struct iFGraph *fgraph
  )
  {
  struct iMachine *ret = (struct iMachine *)malloc(sizeof(struct iMachine));
  struct test_M_and_C *C = (struct test_M_and_C *)(self->C);
  C->process = process;
  ret->M = C;
  ret->run = test_imachine_run;
  ret->destroy = test_imachine_destroy;
  return ret;
  }

/*end implementations*/

void pack_dictionary(int size, int *numbers, char *names)
  {
  int index = 0;
  int i;
  for(i=0; i<size; i++)
    {
    numbers[i] = get_name(names+index);
    index = index + strlen(names+index)+1;
    }
  }

void test_dump_variables(
  struct iContext *cntx,
  long int *variables    
)
{
  int len = cntx->i_namespace->get_size(cntx->i_namespace);
  int i;
  for(i = 0; i < len; i++)
    {
    printf("%d %s = %d\n", i, decode_name(variables[i]), variables[i]);
    } 
}

struct fnode *test_make_fnode(
  struct iFGraph *fg,
  void *f(long int *a),
  int num_parameters,
  int num_locals,
  int num_returns,
  char *namespace
  )
  {
  struct iContext *context = new_context();  
  struct iCode *code = (struct iCode *)malloc(sizeof(struct iCode));
  struct test_M_and_C *C = (struct test_M_and_C *)malloc(sizeof(struct test_M_and_C));
  int numspace[num_parameters+num_returns+num_locals];
  context->i_namespace->size_namespace(context->i_namespace, num_parameters, num_locals, num_returns, 0);
  pack_dictionary(num_parameters+num_returns+num_locals, numspace, namespace);
  context->i_namespace->write_namespace(context->i_namespace, numspace, numspace+num_parameters, numspace+num_locals+num_parameters );
  debug_describe_fcontext(context);
  C->f = f;
  C->cntx = context;
  C->fgraph = fg;
  code->C = C;
  code->new = test_icode_new;
  context->append_codeblock(context, code); 
  return fg->ground(fg, context);
  } 

void test_set_variable( struct iContext *cntx, long int *variables, char *name, long int value)
  {
  int n = get_name(name);
  long int a = cntx->i_namespace->variable_offset(cntx->i_namespace,n);
  variables[a] = value;
  }

long int test_get_variable( struct iContext *cntx, long int *variables, char *name)
  {
  int n = get_name(name);
  long int a = cntx->i_namespace->variable_offset(cntx->i_namespace,n);
  return variables[a];
  }

void test_apply_variables(
  struct iFGraph *fg,
  struct fnode *f,
  long int value)
  {
  struct _parameter *p = (struct _parameter *)malloc(sizeof(struct _parameter));
  p->curry_flag = 0;
  p->value.value = value;
  p->value.type = &TYPE_SCALAR;
  fg->apply(fg, f, p);
  }
 
