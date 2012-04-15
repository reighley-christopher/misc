#include <stdio.h>
#include "scheme.h"
#include <libguile.h>

#define XSCHEME_SIZE 1024

char xscheme_buffer[ XSCHEME_SIZE];
int xscheme_index;
char *xscheme_stack_top = xscheme_buffer;
/*Scheme_Env *env;*/
int process_constant( char *a, char *b, int *c, int *d );
extern int current_label;

static SCM xscheme_dij_respond( SCM args )
   {
     long n;
     char *s;
     SCM cdr = args;
     printf("LINKER-");
     while(!scm_null_p(cdr))
       {
	 s = SCM_STRING_CHARS( SCM_CAR( cdr ) );
         printf("%s ", s);
         cdr = SCM_CDR(cdr);
       }
     printf("\n");
     return 0;
   }

SCM xscheme_dij_constant(SCM stype, SCM encoding)
   {
   /*takes two string parameters, type and encoding, calls process constant*/
   char *type;
   char *buffer;
   int constant;
   int constant_buffer[1024];
   int length;
   int i = 0;
   long n;
   type = SCM_STRING_CHARS( stype );
   buffer = SCM_STRING_CHARS( encoding );
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
   return scm_int2num( current_label-1 );
   }

SCM code_tree( SCM list);

SCM xscheme_prettyprint( SCM v )
{
  char *s;
  long n;
  int indent = 0;
  long index = 0;
  int i;
  SCM str;
  str = scm_object_to_string( v, scm_c_eval_string("write") );
  s = SCM_STRING_CHARS( str );
  n = SCM_STRING_LENGTH( str );
  printf("%s %d\n", s, n );
  while( index < n )
    {
      while( s[index] != '(' && s[index] != ')' && s[index] != ' ' ) 
         { 
         putchar(s[index]); 
         if(s[index] == '\"') 
            {
            index = index+1;
            while(s[index] != '\"') { putchar(s[index]); index = index+1; }
            putchar(s[index]);
            }
         index = index + 1; 
         }
      if( s[index] == ' ' && s[index-1] != ' ') { printf("\n"); }
      if ( s[index] == ')' ) { indent = indent-1; };
      if ( s[index] == '(' ) { indent = indent+1; };
      if ( s[index+1] != '(' && s[index+1] != ')' && s[index+1] != ' ' )
        {
	  for( i=0; i < indent; i++  ) { printf("...|"); }  
        }
      index = index+1;

    }
   printf("\n");
   return 0;
}

/*
SCM xscheme_dij_print(SCM argv)
   {
   int i;
   for(i = 0; i < argc; i++ )
      {
      xscheme_prettyprint( argv );
      }
   return 0;
   }
*/

void code_element( SCM element )
   {
   /*support function for xscheme_dij_code, processes one element
     of the code tree, possibly calling code_tree recursively*/
   if( scm_pair_p(element) == SCM_BOOL_T )
      { 
      code_tree( element );
      } else if (scm_integer_p(element) == SCM_BOOL_T ) {
      printf("REFERENCE-GLOBAL\n%d\n", scm_num2int(element, 0, "code_element"));
   } else { printf("CURRY\n"); } 
   /*anything not an integer and not a list is a place holder*/
   /*note: this doesn't really work the way it should, I'll have to come back to it*/
   /*TODO review code_element in the scheme.c module*/
   }

SCM code_tree( SCM list )
   {
   /*support function for xscheme_dij_code, the parameter is a list
     this function will be called recursively on any element of the list
     that is also a list*/
   SCM car = SCM_CAR( list );
   SCM cdr = SCM_CDR( list );
   code_element( car );
   while (scm_pair_p(cdr) == SCM_BOOL_T )
      {
      code_element(SCM_CAR( cdr ));
      cdr = SCM_CDR( cdr );
      printf("APPLY\n");
      }
   printf("CURRY\nCHECK\n");
   return 0;
   }

SCM xscheme_dij_code(SCM ct)
   {
   /*takes as its argument a tree of numbers representing entries in the 
     constant table, to be translated as an s-expression*/
   printf("PUSH-CODE\n");
   code_tree( ct );
   printf("HALT\nPOP-CODE\n1\nDEFINE-GLOBAL\n");
   return 0;
   }

/*this is a workaround so that I can use namespace-defined to find the correct value for dij-respond and others*/
SCM xscheme_namespace_defined( SCM arg )
   {
   return SCM_BOOL_T;
   }


void xscheme_init()
   {
   /*Scheme_Object *responder;
   Scheme_Object *defined;
   Scheme_Object *constant;
   Scheme_Object *code;
   Scheme_Object *print; */
   /*scheme_set_stack_base(NULL, 1);*/
   /*env = scheme_basic_env();*/
   
   scm_c_define_gsubr("namespace-defined?", 1,0,0, xscheme_namespace_defined);
   scm_c_define_gsubr("dij-respond", 0,0,1, xscheme_dij_respond);
   scm_c_define_gsubr("dij-constant", 2,0,0, xscheme_dij_constant);
   scm_c_define_gsubr("dij-code", 1,0,0, xscheme_dij_code);
   scm_c_define_gsubr("dij-print", 1,0,1, xscheme_prettyprint);

   /*
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
   */

   xscheme_file("scheme_core.scheme");
   /*if( !env ) { printf("scheme error\n"); }*/
   xscheme_index = 0;
   }

void xscheme_consume(char c)
   {
   xscheme_buffer[xscheme_index] = c;
   xscheme_index = xscheme_index+1;
   }

char *xscheme_push()
   {
   char *ret = xscheme_stack_top;
   xscheme_buffer[xscheme_index] = '\0';
   xscheme_index = xscheme_index + 1;
   xscheme_stack_top = xscheme_buffer + xscheme_index;
   return ret;
   }

/*TODO handle comments in xscheme_file*/
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
	     scm_c_eval_string( buffer );
             while( c != '(' && c != EOF ) { c = fgetc( scheme_source ); }
	   } else { c = fgetc( scheme_source ); }
           if( i > 4096 )
               {
                 buffer[50] = '\0';
                 printf
                    (
                    "could not process scheme file," 
                    "S-expression exceeds 4096 bytes :\n %s ...\n", 
                    buffer
                    );
		 exit(0);
               }
       }
     fclose( scheme_source );
  
   }

void xscheme_push_scope()
{
  scm_c_eval_string("(dij-push-scope)");
}

void xscheme_pop_scope()
{
  scm_c_eval_string("(dij-pop-scope)");
}

void xscheme_node( char *name, int count )
{
  char buffer[100];
  sprintf( buffer, "(dij-node '%s %d)", name, count );
  scm_c_eval_string(buffer);
}

void xscheme_leaf( char *name, char *parameter )
{
  char buffer[100];
  sprintf( buffer, "(dij-leaf '%s '%s)", name, parameter );
  scm_c_eval_string(buffer);
}

void xscheme_list( char *name )
{
  char buffer[100];
  sprintf( buffer, "(dij-list '%s)", name );
  scm_c_eval_string(buffer);
}

void xscheme_ground( char *name )
{
  char buffer[100];
  sprintf( buffer, "(dij-ground '%s)", name );
  scm_c_eval_string(buffer);
}

void xscheme_eval( char *index )
   {
   SCM v;
   long n;
   if( !index ) { return; }
   if( xscheme_stack_top < index ) 
      { 
      printf("scheme parsing error %d < %d\n", xscheme_stack_top, index); 
      }
   v = scm_c_eval_string(index);
   /*xscheme_prettyprint( v );*/
   xscheme_stack_top = index;
 
   xscheme_index = index-xscheme_buffer;
   }


void xscheme_complete()
   {
   scm_c_eval_string("(dij-complete)");
   }
