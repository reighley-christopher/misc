#include <stdio.h>

FILE *yyin;

void yyerror( const char *message )
{
  fprintf(stderr, "syntax error : %s\n", message);
}

int main( int argc, char *argv[] )
     {
       int token;
       yyin = fopen(argv[1], "r");
       xscheme_init();
       semantic_init( 4096 );
       yyparse();
     }
