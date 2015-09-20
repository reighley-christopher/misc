#include <stdio.h>
#include <stdlib.h>
#include "../../util/dij_misc.h"
#include "../../control/dij_control.h"
#include "fixtures.h"


void *assign_1_to_a( struct iContext *cntx, struct iProcess *process, struct iFGraph * fgraph)
  {
  long int *variables;
  printf("assign_1_to_a\n");
  process->get_memory(process, &variables, 0, 0);
  test_set_variable(cntx, variables, "a", 1);
  printf("finish\n");
  return 0;
  }

void *assign_a_to_b( struct iContext *cntx, struct iProcess *process, struct iFGraph * fgraph)
  {
  long int *variables;
  long int a, b;
  printf("assign_a_to_b\n");
  process->get_memory(process, &variables, 0, 0);
  printf("proc variables %x\n", variables);
  a = test_get_variable(cntx, variables, "a");  
  printf("a = %d\n", a); 
  test_set_variable(cntx, variables, "b", a);
  printf("finish\n");
  return 0;
  }

void *return_a( struct iContext *cntx, struct iProcess *process, struct iFGraph * fgraph)
  { 
  long int *variables;
  printf("return_a\n");
  process->get_memory(process, &variables, 0, 0);
  test_set_variable(cntx, variables, "return", test_get_variable(cntx, variables, "a"));
  return 0;
  }

int main( int argc, char *argv )
  {
  /*
  testing the njoin method on fgraph
  case1 :
   f := { a := 1 };
   g := { b := a }.2;
   h := { return := a };
   z := (f:g:h);
  */
  struct iProcess *proc;
  struct iFGraph *fg = fgraph_new();
  struct fnode *f = test_make_fnode(fg, assign_1_to_a, 0, 0, 1, "a");
  struct fnode *g = test_make_fnode(fg, assign_a_to_b, 1, 0, 1, "a\0b");
  struct fnode *h = test_make_fnode(fg, return_a, 1, 0, 1, "a\0return"); 
  struct fnode *z;
  g = test_apply_variables(fg, g, 2); 
  z = fg->njoin(fg, f, g);
  z = fg->njoin(fg, z, h);
  proc = fg->load(fg, z, 0, 0);
  proc->attach(proc);
  process_go();
  return 0;
  } 
