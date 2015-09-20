%option noyywrap

%{
#include "dij_misc.h"
#include "../control/dij_control.h"
#include "dij_exec.h"

typedef long int DIJ_WORD;

%}

%%

[\t\n ]*   { }

[0-9]*  { DIJ_WORD d; sscanf(yytext, "%ld", &d ); dij_write(d); dij_record_number( d ); }

"PUSH" { dij_write((long int)inst_push); }

"BLANK-CALL" {} 

"APPLY" { dij_write((long int)inst_apply); }

"CURRY" { dij_write((long int)inst_curry); }

"CHECK" { dij_write((long int)inst_check); }

"HALT" { dij_write((long int)inst_halt); }

"PUSH-CODE" { dij_push_code();  }

"POP-CODE" { dij_pop_code(); printf("'pop_code\n");}

"PUSH-DATA" { dij_push_data(); }

"POP-DATA" { dij_pop_data(); }

"REFERENCE-VARIABLE" { dij_write((long int)inst_reference_variable); dij_note_number( dij_note_rvalue ); } 

"DEFINE-GLOBAL" {dij_define_global(); }

"REFERENCE-GLOBAL" { dij_write((long int)inst_reference_global); }

"ASSIGN" { dij_write((long int)inst_assign); }

"ADD" { dij_write((long int)inst_add); }

"LVALUE-NAME" {dij_write((long int)inst_lvalue_name); dij_note_number( dij_note_lvalue ); }

"LVALUE-CALL" {dij_write((long int)inst_lvalue_call); }

"ANONYMOUS-PARAMETER" {dij_write((long int)inst_anonymous_parameter); }

"ANONYMOUS-RETURN" {dij_write((long int)inst_anonymous_return); }

"SPAWN" {dij_write((long int)inst_spawn); }

"DEBUG" {dij_write((long int)debug); }

"LEFT-JOIN" { dij_write((long int)inst_left_join); }

"NAMESPACE-JOIN" {dij_write((long int)inst_namespace_join); }

"PUSH-IF" {dij_push_if(); }

"PUSH-DO" {dij_push_do(); }

"TEST" {dij_control_test(); }

"MARK" {dij_control_marker(); }

"POP-CONTROL" {dij_pop_control(); }

"NOT" { dij_write((long int)inst_not); }

"NEG" { dij_write((long int)inst_neg); }

"EQUAL" { dij_write((long int)inst_equal); }

<<EOF>> {printf("finished\n"); yyterminate(); }

