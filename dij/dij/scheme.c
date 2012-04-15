#include <stdio.h>
#include "scheme.h"
#include <scheme.h>

#define XSCHEME_SIZE 1024

char xscheme_buffer[ XSCHEME_SIZE];
int xscheme_index;
char *xscheme_stack_top = xscheme_buffer;
Scheme_Env *env;
int process_constant( char *a, char *b, int *c, int *d );
extern int current_label;

Scheme_Object *xscheme_dij_respond(int argc, Scheme_Object **argv)
   {
     int i;
     Scheme_Object ret;
     long n;
     char *s;
     printf("LINKER-");
     for(i = 0; i < argc; i++)
       {
	 s = scheme_display_to_string( argv[i], &n );
         printf("%s ", s);
       }
     printf("\n");
     return scheme_null;
   }

Scheme_Object *xscheme_dij_constant(int argc, Scheme_Object **argv)
   {
   /*takes two string parameters, type and encoding, calls process constant*/
   char *type;
   char *buffer;
   int constant;
   int constant_buffer[1024];
   int length;
   int i = 0;
   long n;
   type = scheme_display_to_string( argv[0], &n );
   buffer = scheme_display_to_string( argv[1], &n );
   length = process_constant( type, buffer, &constant, constant_buffer );
   if( !length )
      {
      printf("%d\n%d\nDEFINE-GLOBAL\n", constant, current_label); 
      } else {
      printf("PUSH-DATA\n");
      for(i = 0; i < length; i++) {
      printf("%d\n", constant_buffer[i] ); }
      printf("POP-DATA\n%d\nDEFINE-GLOBAL\n", current_label);
      } 
   current_label = current_label+1;
   return scheme_make_integer( current_label-1 );
   }

void code_tree( Scheme_Object *list);

void xscheme_prettyprint( Scheme_Object *v )
{
  char *s;
  long n;
  int indent = 0;
  long index = 0;
  int i;
  s = scheme_write_to_string( v, &n );
  printf("%s\n", s );
  while( index < n )
    {
      while( s[index] != '(' && s[index] != ')' && s[index] != ' ' ) { 
      putchar(s[index]); 
      if(s[index] == '\"') { 
         index = index+1;
         while(s[index] != '\"') { putchar(s[index]); index = index+1; }
         putchar(s[index]);
      } 
      index = index + 1; }
      if( s[index] == ' ' && s[index-1] != ' ') { printf("\n"); }
      if ( s[index] == ')' ) { indent = indent-1; };
      if ( s[index] == '(' ) { indent = indent+1; };
      if ( s[index+1] != '(' && s[index+1] != ')' && s[index+1] != ' ' )
        {
	  for( i=0; i < indent; i++  ) { printf("...|"); }  
        }
      index = index+1;

    }
}

Scheme_Object *xscheme_dij_print(int argc, Scheme_Object **argv)
   {
   int i;
   for(i = 0; i < argc; i++ )
      {
      xscheme_prettyprint( argv[i] );
      }
   return scheme_null;
   }

void code_element( Scheme_Object *element )
   {
   /*support function for xscheme_dij_code, processes one element
     of the code tree, possibly calling code_tree recursively*/
   if( SCHEME_PAIRP(element) )
      {
      code_tree( element );
      } else if (SCHEME_INTP(element)) {
      printf("REFERENCE-GLOBAL\n%d\n", SCHEME_INT_VAL(element));
   } else { printf("CURRY\n"); } /*anything not an integer and not a list is a place holder*/ /*note: this doesn't really work the way it should, I'll have to come back to it*/
   }

void code_tree( Scheme_Object *list )
   {
   /*support function for xscheme_dij_code, the parameter is a list
     this function will be called recursively on any element of the list
     that is also a list*/
   Scheme_Object *car = SCHEME_CAR( list );
   Scheme_Object *cdr = SCHEME_CDR( list );
   code_element( car );
   while (SCHEME_PAIRP(cdr))
      {
      code_element(SCHEME_CAR( cdr ));
      cdr = SCHEME_CDR( cdr );
      printf("APPLY\n");
      }
   printf("CURRY\nCHECK\n");
   }

Scheme_Object *xscheme_dij_code(int argc, Scheme_Object **argv)
   {
   /*takes as its argument a tree of numbers representing entries in the 
     constant table, to be translated as an s-expression*/
   printf("PUSH-CODE\n");
   code_tree( argv[0] );
   printf("HALT\nPOP-CODE\n1\nDEFINE-GLOBAL\n");
   return scheme_null;
   }

/*this is a workaround so that I can use namespace-defined to find the correct value for dij-respond and others*/
Scheme_Object *xscheme_namespace_defined(int argc, Scheme_Object **argv)
   {
   return scheme_true;
   }

void xscheme_init()
   {
   Scheme_Object *responder;
   Scheme_Object *defined;
   Scheme_Object *constant;
   Scheme_Object *code;
   Scheme_Object *print;
   scheme_set_stack_base(NULL, 1);
   env = scheme_basic_env();
   defined = scheme_make_prim_w_arity(xscheme_namespace_defined, "namespace-defined?", 1, 1);
   responder = scheme_make_prim_w_arity(xscheme_dij_respond, "dij-respond", 0, -1);
   constant = scheme_make_prim_w_arity(xscheme_dij_constant, "dij-constant", 2, 2);
   code = scheme_make_prim_w_arity(xscheme_dij_code, "dij-code", 1, 1);
   print = scheme_make_prim_w_arity(xscheme_dij_print, "dij-print", 1, -1 );
   scheme_add_global("dij-respond", responder, env);
   scheme_add_global("namespace-defined?", defined, env);
   scheme_add_global("dij-constant", constant, env);
   scheme_add_global("dij-code", code, env);
   scheme_add_global("dij-print", print, env);
   xscheme_file("scheme_core.scheme");
   if( !env ) { printf("scheme error\n"); }
   xscheme_index = 0;
   }




void xscheme_consume(char c)
   {
   xscheme_buffer[xscheme_index] = c;
   xscheme_index = xscheme_index+1;
   }

void *xscheme_push()
   {
   char *ret = xscheme_stack_top;
   xscheme_buffer[xscheme_index] = '\0';
   xscheme_index = xscheme_index + 1;
   xscheme_stack_top = xscheme_buffer + xscheme_index;
   return ret;
   }

void xscheme_file( char *filename )
   {
     char buffer[4096];
     int c;
     int i = 0;
     int parens = 0;
     FILE *scheme_source = fopen( filename, "r" );
     c = fgetc( scheme_source );
     while( c != EOF )
       {
         if( c == '(') { parens = parens+1; }
         if( c == ')') { parens = parens-1; }
	 buffer[i] = c;
         i = i + 1;
         if( parens == 0 )
	   {
             buffer[i] = '\0';
             i = 0;
	     scheme_eval_string_all( buffer, env, 1 );
             while( c != '(' && c != EOF ) { c = fgetc( scheme_source ); }
	   } else { c = fgetc( scheme_source ); }
           if( i > 4096 )
               {
                 buffer[50] = '\0';
                 printf("could not process scheme file, S-expression exceeds 4096 bytes :\n %s ...\n", buffer);
		 exit(0);
               }
       }
     fclose( scheme_source );
  
   }

void xscheme_node( char *name, int count )
{
  char buffer[100];
  sprintf( buffer, "(dij-node '%s %d)", name, count );
  scheme_eval_string(buffer, env);
}

void xscheme_leaf( char *name, char *parameter )
{
  char buffer[100];
  sprintf( buffer, "(dij-leaf '%s '%s)", name, parameter );
  scheme_eval_string(buffer, env);
}

void xscheme_list( char *name )
{
  char buffer[100];
  sprintf( buffer, "(dij-list '%s)", name );
  scheme_eval_string(buffer, env);
}

void xscheme_ground( char *name )
{
  char buffer[100];
  sprintf( buffer, "(dij-ground '%s)", name );
  scheme_eval_string(buffer, env);
}

void xscheme_eval( char *index )
   {
   Scheme_Object *v;
   long n;
   if( !index ) { return; }
   if( xscheme_stack_top < index ) { printf("scheme parsing error %d < %d\n", xscheme_stack_top, index); }
   v = scheme_eval_string(index, env);
   /*xscheme_prettyprint( v );*/
   xscheme_stack_top = index;
 
   xscheme_index = index-xscheme_buffer;
   }


void xscheme_complete()
   {
   scheme_eval_string("(dij-complete)", env);
   }
