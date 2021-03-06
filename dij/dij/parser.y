%{

#include <stdio.h>
#include <string.h>
#include "scheme.h"
#include "tree.h"
  /*#include "semantic.h"*/

#define YYSTYPE struct _tree *

extern char *yytext;
extern int constant;
extern int const_buffer[];
extern int const_size;

char name_table[1024];
int name_table_size = 0;
int current_label = 2;

int get_name( char *name )
{
  char *string =  name_table;
  int ret = 0;
  int offset = 0;
  while( offset < name_table_size )
    {
      if( !strcmp( string, name ) ) { 
         return ret; 
         }

      offset = offset + strlen( string ) + 1;
      ret = ret+1;
      string = string + strlen( string ) + 1;
    }
  strcpy( string, name );
  name_table_size = name_table_size + strlen( string ) + 1;
  return ret;
}

char *decode_name( int code )
{
  char *ret = name_table;
  while( code != 0 )
  {
    ret = ret + strlen( ret ) + 1;
    code = code-1;
  }
  return ret;
}

struct _constant_block
{
   int size;
   int *data;
};

struct _constant_block *create_constant_block( int size, int *data )
{
   struct _constant_block *ret;
   int i;
   ret = malloc( sizeof(struct _constant_block) );
   ret->data = malloc( sizeof(int) * size );
   ret->size = size;
   for( i = 0; i < size; i++ ) { ret->data[i] = data[i]; }
   return ret;
}

void nil( int arg ) { /*printf(".\n");*/ }
void dump( int arg ) { printf( (char *)arg ); }
void literal( int arg ) { printf("%d\n", arg ); }
void push_literal(int arg) {printf("PUSH\n%d\n", arg ); }
void define( int arg) { printf("%d\nDEFINE-GLOBAL\n", arg); }
void begin_code( int arg ) { printf("PUSH-CODE\n"); }
void end_code( int arg ) { printf("CHECK\nHALT\nPOP-CODE\n%d\nDEFINE-GLOBAL\n", arg); }
void name( int arg ) { printf("NAME %d : %s\n", arg, decode_name(arg) ); }
void constant_block( int arg ) 
{
   struct _constant_block *cb = (struct _constant_block *)arg;
   int i;
   printf("PUSH\nPUSH-DATA\n");
   for(i = 0; i < cb->size; i++)
   {
      printf("%d\n", cb->data[i]);
   }
   printf("POP-DATA\n");
   free(cb->data);
   free(cb);
}
%}

%token TOKEN_EOF
%token TOKEN_WORD 		 
%token TOKEN_DECIMAL_INTEGER 
%token TOKEN_SCHEME_SCRIPT 
%token TOKEN_OPEN_BLOCK 
%token TOKEN_CLOSE_BLOCK 
%token TOKEN_OPEN_PAREN 
%token TOKEN_CLOSE_PAREN 
%token TOKEN_OPEN_BRACKET
%token TOKEN_CLOSE_BRACKET
%token TOKEN_DOT
%token TOKEN_COLON
%token TOKEN_BACKSLASH
%token TOKEN_TILDE 		 
%token TOKEN_COMMA 
%token TOKEN_SEMICOLON 
%token TOKEN_ADD 
%token TOKEN_SUBTRACT 
%token TOKEN_MULTIPLY 
%token TOKEN_DIVIDE 
%token TOKEN_MODULO 
%token TOKEN_NOT 
%token TOKEN_OR 		
%token TOKEN_AND 
%token TOKEN_GUARD 
%token TOKEN_REFERENCE 
%token TOKEN_DEREFERENCE 
%token TOKEN_ASSIGN 
%token TOKEN_IF
%token TOKEN_FI
%token TOKEN_DO
%token TOKEN_OD
%token TOKEN_SCHEME
%token TOKEN_END_GUARD
%token TOKEN_FUNCTION_APPLICATION
%token TOKEN_CONSTANT
%token TOKEN_TEST_EQUALITY

%start function_collection

%%

function_collection:
{
  xscheme_complete();
}
|
exterior_function_constant function_collection
{
  execute_tree( $1 );
}

exterior_function_constant:
scheme begin_function command_list end_function
{
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  char buffer[5];
  sprintf(buffer, "%d", current_label );
  $$ = create_tree( end_code, current_label, create_tree(begin_code, 0, 0, 0), $3 );
  current_label = current_label+1;
  xscheme_leaf("label", buffer);
  xscheme_node( "block", 2 ); 
  xscheme_eval( $1 );
}
;

function_constant:
scheme begin_function command_list end_function
{
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  $$ = create_tree( dump, "CHECK\nHALT\nPOP-CODE\n", create_tree(dump, "PUSH\nPUSH-CODE\n", 0, 0), $3 );
  xscheme_node( "block", 1 ); 
  xscheme_eval( $1 );
}
;

scheme:
{ $$ = 0; }
|
TOKEN_SCHEME scheme
{$$ = xscheme_push(); }
;

begin_function:
TOKEN_OPEN_BLOCK
{

}
;

command_list:
{
  xscheme_ground("command-list");
  $$ = 0;
}
|
command
{ 
   xscheme_ground("command-list");
   xscheme_list("command-list");
   $$ = $1;
}
|
command_list TOKEN_SEMICOLON command
{
  xscheme_list("command-list");
  $$ = create_tree( nil, 0, $1, $3 );
}
;

end_function:
TOKEN_CLOSE_BLOCK
{
}
;

command:
lvalue_list TOKEN_ASSIGN expression_list
{
  xscheme_node("assignment", 2);
  $$ = create_tree( dump, "ASSIGN\n", $1, $3 );
}
|
terminated_combinator_expression
{
  xscheme_node("function-call", 1);
  $$ = create_tree( dump, "CHECK\n", $1, 0 );
}
|
if_statement
{
  xscheme_node("command", 0);
  $$ = $1;
}
|
do_statement
{
  xscheme_node("command", 0);
  $$ = $1;
}
;

lvalue_list: 
{
  xscheme_ground("lvalue-list");
  $$ = create_tree(dump, "ANONYMOUS-RETURN\n", 0, 0);
}
|
lvalue
{
  xscheme_ground("lvalue-list");
  xscheme_list("lvalue-list");
  $$ = $1;
}
|
lvalue TOKEN_COMMA lvalue_list
{
  xscheme_list("lvalue-list");
  $$ = create_tree( nil, 0, $1, $3 );
}
;

expression_list:
{
  xscheme_ground("expression-list");
  $$ = create_tree(dump, "ANONYMOUS-PARAMETER\n", 0, 0);
}
|
expression
{
  xscheme_ground("expression-list");
  xscheme_list("expression-list");
  $$ = $1;
}
|
expression TOKEN_COMMA expression_list
{
  xscheme_list("expression-list");
  $$ = create_tree( dump, "EXPRESSION LIST\n", $1, $3 );
  if( $1 == 0 || $3 == 0 ) { printf("one of the list items is 0\n"); }
}
;

if_statement:
TOKEN_IF guarded_command_list TOKEN_FI
{
  $$ = create_tree( nil, 0, $1, 0 );
  xscheme_node("if", 1 );
}
;

do_statement:
TOKEN_DO guarded_command_list TOKEN_OD
{
  $$ = create_tree( nil, 0, $1, 0 );
  xscheme_node( "do", 1 );
}
;

lvalue:
scheme_name
{
  xscheme_node("lvalue-name", 1);
  $$ = create_tree( nil, 0, create_tree( dump, "LVALUE-NAME\n", 0, 0 ), $1 );
}
|
TOKEN_DEREFERENCE atomic_expression
{
  printf("dereference\n");
  xscheme_node("lvalue-address", 0);
  $$ = create_tree( dump, (int)"LVALUE-ADDRESS\n", $2, 0 );
}
|
terminated_combinator_expression
{
  xscheme_node("lvalue-call", 1);
  $$ = create_tree( dump, (int)"LVALUE-CALL\n", $1, 0 );
}
;

guarded_command_list:
{
  xscheme_ground("guarded-command-list");
}
|
guarded_command guarded_command_list
{
  xscheme_list("guarded_command_list");
}
;

guarded_command :
expression TOKEN_GUARD command_list TOKEN_END_GUARD
{
  create_tree( nil, 0, $1, $3 );
  xscheme_node("guarded-command", 2);
}
;

tier0_expression :
TOKEN_DEREFERENCE atomic_expression
{
  printf("dereference\n");
}
|
atomic_expression
{
  $$ = $1;
}
;

uninitiated_combinator_expression:
TOKEN_FUNCTION_APPLICATION proper_combinator_expression
{
}
|
TOKEN_FUNCTION_APPLICATION uninitiated_combinator_expression
{
}
;

terminated_combinator_expression:
proper_combinator_expression TOKEN_FUNCTION_APPLICATION
{
$$ = create_tree( dump, "CURRY\n", $1, 0 );
xscheme_node("curry", 1);
}
|
terminated_combinator_expression TOKEN_FUNCTION_APPLICATION
{
$$ = create_tree( dump, "CURRY\n", $1, 0 );
xscheme_node("curry", 1);
}
;

proper_combinator_expression:
tier0_expression
{
$$ = $1;
}
|
proper_combinator_expression TOKEN_FUNCTION_APPLICATION tier0_expression
{
$$ = create_tree( dump, "APPLY\n", $1, $3 );
xscheme_node("apply", 2 );
}
|
terminated_combinator_expression TOKEN_FUNCTION_APPLICATION tier0_expression
{
$$ = create_tree( dump, "APPLY\n", $1, $3 );
 xscheme_node("apply", 2 );
}
|
proper_combinator_expression TOKEN_COLON tier0_expression
{
  $$ = create_tree( dump, "NAMESPACE-JOIN\n", $1, $3);
  xscheme_node("namespace-join", 2);
}
;

functional_expression:
proper_combinator_expression
{
$$ = $1;
}
|
terminated_combinator_expression
{
$$ = $1;
}
|
uninitiated_combinator_expression
{
}
|
functional_expression TOKEN_BACKSLASH atomic_expression
{
  $$ = create_tree( dump, "LEFT-JOIN\n", $1, $3 );
  xscheme_node( "left-join", 2 );
}
;

tier1_expression :
TOKEN_REFERENCE tier1_expression
{
  printf("reference\n");
}
|
TOKEN_NOT tier1_expression
{
  printf("not\n");
}
|
TOKEN_SUBTRACT tier1_expression
{
  printf("negate\n");
}
|
TOKEN_TILDE tier1_expression
{
  $$ = create_tree( dump, "SPAWN\n", $2, 0 );
  xscheme_node( "spawn", 1 );
}
|
functional_expression
{
  $$ = create_tree( dump, "CHECK\n", $1, 0 );
}
;

tier2_expression :
TOKEN_ADD expression
{
  $$ = create_tree(dump, "ADD\n", $2, 0 );
  xscheme_node ( "+", 2 );
}
;

tier3_expression :
TOKEN_MULTIPLY expression
{
  printf("multiply\n");
  xscheme_node( "*", 2 );
}
|
TOKEN_DIVIDE expression
{
  xscheme_node( "/", 2 );
}
|
TOKEN_MODULO expression
{
  xscheme_node( "%", 2 );
}
|
tier2_expression
{
   $$ = $1;
}
;

tier3_point_5_expression :
TOKEN_TEST_EQUALITY expression
{
  $$ = create_tree( dump, "EQUAL\n", $2, 0 ); 
  xscheme_node( "eq?", 2 );
}
|
tier3_expression
{
  $$ = $1;
}

tier4_expression :
TOKEN_AND expression
{
  xscheme_node( "and", 2 );
}
|
tier3_point_5_expression
{
  $$ = $1;
}
;

tier5_expression :
TOKEN_OR expression
{
  xscheme_node("or", 2);
}
|
tier4_expression
{
  $$ = $1;
} 
;

expression :
tier1_expression 
{
  $$ = $1;
  xscheme_node("expression", 1);
}
|
tier1_expression tier5_expression
{
  xscheme_node("expression", 1);
  $$ = create_tree( nil, 0, $1, $2);
}
;

atomic_expression:
TOKEN_DECIMAL_INTEGER
  {
    int n;
    sscanf( yytext, "%d", &n );
    $$ = create_tree( push_literal, n , 0, 0 );
    xscheme_leaf("constant", yytext);
  }
|
TOKEN_CONSTANT
  {
    if( !const_size )
      {
      $$ = create_tree( literal, constant, 0, 0 );
      } else {
      $$ = create_tree( constant_block, create_constant_block(const_size, const_buffer), 0, 0 );
      }
    xscheme_node("const", 2);
  }
|
function_constant
{
  $$ = $1;
}
|
scheme_name
  {
    $$ = create_tree( nil, 0, create_tree( dump, "REFERENCE-VARIABLE\n", 0, 0 ), $1 );
  }
|
TOKEN_OPEN_PAREN expression TOKEN_CLOSE_PAREN
{
  $$ = $2;
}
;

scheme_name:
scheme TOKEN_WORD
{
   char offset_s[10];
   int offset_d = get_name(yytext);
   sprintf(offset_s, "%d", offset_d);
   $$ = create_tree(literal, offset_d, 0, 0);
   xscheme_leaf("name", yytext);
   xscheme_leaf("offset", offset_s);
   xscheme_node("identifier", 2);
   xscheme_eval( $1 );
}
;
