#include <stdio.h>

FILE *yyin;

void yyerror( const char *message )
{
  fprintf(stderr, "syntax error : %s\n", message);
}

void inner_main( void *closure, int argc, char **argv )
{
       int token;
       yyin = fopen(argv[1], "r");
       xscheme_init();
       semantic_init( 4096 );
       yyparse();
}

int main( int argc, char **argv )
{
   scm_boot_guile( argc, argv, inner_main, 0);
}

