%{

#include <stdio.h>
#include "y.tab.h"
#include "scheme.h"

int process_constant( char *a, char *b, int *c, int *d );
int constant;
int const_buffer[1024];
int const_size;

%}

if                       "if"
fi                       "fi"
do                       "do"
od                       "od"

word			 ([A-Z]|[a-z]|_)([A-Z]|[a-z]|_|[0-9])*
decimal_integer	         ([0-9])*
whitespace		 ([ \t])+
comment			 "/*"
scheme_script		 "!!("
open_block		 "{"
close_block		 "}"
open_paren		 "("
close_paren		 ")"
semicolon		 ";"
add			 "+"
subtract		 "-"
multiply		 "*"
divide		 	 "/"
modulo			 "%"
not			 "!"
or			 "|"
and			 "&"
guard			 "->"
reference		 "#"
dereference		 "@"
assign                   ":="
comma                    ","
end_guard                "[]"
function_application     "."
constant                 "["[^\]:\n]*":"
join                     "\\"
colon                    ":"
test_equality            "=="
spawn                    "~"
%%

{do}            { return TOKEN_DO; }
{od}            { return TOKEN_OD; }
{if}            { return TOKEN_IF; }
{fi}            { return TOKEN_FI; }

{scheme_script} {
		char ch;
                int paren = 1;
                xscheme_consume('(');
		while(1) {
		   ch = input();
                   xscheme_consume(ch);
		   if(ch==')') paren = paren-1;
                   if(ch=='(') paren = paren+1;
                   if(paren == 0) break;
		   }              
		return TOKEN_SCHEME;
		}

{constant} {
           char ch;
           int i = 0;
           int j = 0;
           int k = 0;
           char buffer[1024];
           while(1) {
                    ch = input();
                    if( ch == ']' ) { buffer[i] = '\0'; break; }
                    buffer[i] = ch;
                    i = i+1;
                    }
           j = i*2+1;
           buffer[j+1] = '\"';
           buffer[j+2] = '\0';
           buffer[i+1] = '\"';
           while(j > i+1) {
                    buffer[j] = buffer[j-i-2];
                    j = j-1;
                    }
           k = i*2+4;
           i = 1;
           while(1) {
                    ch = yytext[i];
                    if( ch == ':' ) { buffer[k] = '\0'; break; }
                    buffer[k] = ch;
                    i = i+1;
                    k = k+1;
                    }
           /*printf("buffer %s\n+1%s\n+2%s\n", buffer, buffer+j, buffer+(j*2)+2 );*/
           xscheme_leaf("type", buffer+(j*2)+2);
           xscheme_leaf("encoding", buffer+j );
           const_size = process_constant( yytext, buffer, &constant, const_buffer );
           
           return TOKEN_CONSTANT;
           }

{comment} {
char ch;
char asterisk = 0;
while(1) {
         ch = input();
         if(ch=='/' && asterisk) break;
         if(ch=='*') {asterisk = 1;} 
         }
}

{word}		{ return TOKEN_WORD; }
{decimal_integer}		  	{ return TOKEN_DECIMAL_INTEGER;}
{whitespace}	/* ignore white space */

{open_block}	{ return TOKEN_OPEN_BLOCK; }
{close_block}   { return TOKEN_CLOSE_BLOCK; }
{open_paren}	{ return TOKEN_OPEN_PAREN; }
{close_paren}   { return TOKEN_CLOSE_PAREN; }

{function_application} { return TOKEN_FUNCTION_APPLICATION; }

{comma}		{ return TOKEN_COMMA; }
{semicolon}	{ return TOKEN_SEMICOLON; }

{add}		{ return TOKEN_ADD; }
{subtract}	  	{ return TOKEN_SUBTRACT; }
{multiply}		{ return TOKEN_MULTIPLY; }
{divide}		{ return TOKEN_DIVIDE; }
{modulo}		{ return TOKEN_MODULO; }
{not}		{ return TOKEN_NOT; }
{or}		{ return TOKEN_OR; }
{and}		{ return TOKEN_AND;}
{test_equality} { return TOKEN_TEST_EQUALITY; }
{guard}		{ return TOKEN_GUARD;}
{end_guard}     { return TOKEN_END_GUARD;}

{reference}	{ return TOKEN_REFERENCE; }
{dereference}	{ return TOKEN_DEREFERENCE; }

{assign}        { return TOKEN_ASSIGN; }
{colon}         { return TOKEN_COLON; }
{join}          { return TOKEN_BACKSLASH; }
{spawn}         { return TOKEN_TILDE; }
