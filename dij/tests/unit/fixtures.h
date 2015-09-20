struct fnode *test_make_fnode(
   struct iFgraph *f,
   void *(*func)(
      struct iContext *cntx, 
      struct iProcess *process,
      struct iFGraph *fgraph), 
   int params, 
   int locals, 
   int returns,
   char *names);

void test_set_variable(
   struct iContext *cntx, 
   long int *variables,
   char *var_name,  
   long int *value);

long int test_get_variable(
   struct iContext *cntx, 
   long int *variables,
   char *var_name);

struct fnode *test_apply_variables(
   struct iFGraph *fg,
   struct fnode *f,
   long int value);
 
void test_dump_variables(
   struct iContext *cntx,
   long int *variables
);
