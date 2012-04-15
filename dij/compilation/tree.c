#include <stdlib.h>
#include "tree.h"

struct _tree *create_tree( command f, int arg, struct _tree *left, struct _tree *right)
{
   struct _tree *ret;
   ret = malloc(sizeof(struct _tree));
   ret->f = f;
   ret->arg = arg;
   ret->left = left;
   ret->right = right;
   return ret;
}

void execute_tree( struct _tree *t )
{
   if(t)
   {
   execute_tree( t->left);
   execute_tree(t->right);
   (t->f)(t->arg);
   free(t);
   }
}

