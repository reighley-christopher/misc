#include <stdio.h>
#include <libguile.h>

static SCM test( SCM val )
{
   int v = scm_to_int( val );
   printf("Hello World I am %d today\n", v);
   return SCM_BOOL_F;
}

void inner_main( void *closure, int argc, char **argv )
{
   scm_c_define_gsubr("test", 1, 0, 0, test);
   scm_c_eval_string("(test 5)");
   scm_c_eval_string("(test 6)");
}

int main( int argc, char **argv )
{
   scm_boot_guile( argc, argv, inner_main, 0);
}

