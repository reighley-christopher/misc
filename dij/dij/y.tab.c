#include <sys/cdefs.h>
#ifndef lint
#if 0
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#else
__IDSTRING(yyrcsid, "$NetBSD: skeleton.c,v 1.14 1997/10/20 03:41:16 lukem Exp $");
#endif
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "parser.y"

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
#line 108 "y.tab.c"
#define TOKEN_EOF 257
#define TOKEN_WORD 258
#define TOKEN_DECIMAL_INTEGER 259
#define TOKEN_SCHEME_SCRIPT 260
#define TOKEN_OPEN_BLOCK 261
#define TOKEN_CLOSE_BLOCK 262
#define TOKEN_OPEN_PAREN 263
#define TOKEN_CLOSE_PAREN 264
#define TOKEN_OPEN_BRACKET 265
#define TOKEN_CLOSE_BRACKET 266
#define TOKEN_DOT 267
#define TOKEN_COLON 268
#define TOKEN_BACKSLASH 269
#define TOKEN_TILDE 270
#define TOKEN_COMMA 271
#define TOKEN_SEMICOLON 272
#define TOKEN_ADD 273
#define TOKEN_SUBTRACT 274
#define TOKEN_MULTIPLY 275
#define TOKEN_DIVIDE 276
#define TOKEN_MODULO 277
#define TOKEN_NOT 278
#define TOKEN_OR 279
#define TOKEN_AND 280
#define TOKEN_GUARD 281
#define TOKEN_REFERENCE 282
#define TOKEN_DEREFERENCE 283
#define TOKEN_ASSIGN 284
#define TOKEN_IF 285
#define TOKEN_FI 286
#define TOKEN_DO 287
#define TOKEN_OD 288
#define TOKEN_SCHEME 289
#define TOKEN_END_GUARD 290
#define TOKEN_FUNCTION_APPLICATION 291
#define TOKEN_CONSTANT 292
#define TOKEN_TEST_EQUALITY 293
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    6,    2,    2,    3,    4,    4,    4,
    5,    7,    7,    7,    7,    8,    8,    8,    9,    9,
    9,   11,   12,   13,   13,   13,   15,   15,   18,   19,
   19,   20,   20,   10,   10,   21,   21,   21,   21,   22,
   22,   22,   22,   23,   23,   23,   23,   23,   24,   25,
   25,   25,   25,   26,   26,   27,   27,   28,   28,   14,
   14,   17,   17,   17,   17,   17,   16,
};
short yylen[] = {                                         2,
    0,    2,    4,    4,    0,    2,    1,    0,    1,    3,
    1,    3,    1,    1,    1,    0,    1,    3,    0,    1,
    3,    3,    3,    1,    2,    1,    0,    2,    4,    2,
    1,    2,    2,    2,    2,    1,    3,    3,    3,    1,
    1,    1,    3,    2,    2,    2,    2,    1,    2,    2,
    2,    2,    1,    2,    1,    2,    1,    2,    1,    1,
    2,    1,    1,    1,    1,    3,    2,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    6,    2,    7,    0,   62,    0,
    0,    0,    0,   63,    0,    0,   64,    9,    0,    0,
   14,   15,    0,    0,   31,   36,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   65,   42,    0,    0,    0,
    0,    0,    0,    0,    0,   67,    0,   11,    0,    3,
    0,    0,    0,    0,    0,   47,   46,   45,   44,   30,
    0,   33,    0,   66,    0,    0,    0,    0,    0,    0,
    0,    0,   53,   55,   57,   59,   61,    0,   22,   28,
   23,    0,   10,   12,    0,   38,   18,    0,   39,   37,
   43,   49,   50,   51,   52,   58,   56,   54,    0,    4,
    0,   29,   21,
};
short yydgoto[] = {                                       2,
    3,   15,    8,   16,   50,   17,   18,   19,   84,   34,
   21,   22,   23,   42,   43,   36,   25,   44,   26,   37,
   38,   39,   40,   73,   74,   75,   76,   77,
};
short yysindex[] = {                                   -275,
 -275,    0, -275, -245,    0,    0,    0, -208,    0, -257,
 -252, -257, -257,    0, -171, -254,    0,    0, -261, -253,
    0,    0, -219,    0,    0,    0, -202, -257, -257, -257,
 -257, -252, -140, -253, -210,    0,    0, -202, -198,   36,
    0, -214, -203, -257, -216,    0, -208,    0, -208,    0,
 -257, -128, -127, -128, -128,    0,    0,    0,    0,    0,
 -253,    0, -202,    0, -252, -257, -257, -257, -257, -257,
 -257, -257,    0,    0,    0,    0,    0, -208,    0,    0,
    0, -254,    0,    0, -195,    0,    0, -253,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -222,    0,
 -257,    0,    0,
};
short yyrindex[] = {                                      1,
 -152,    0,    1,    0,    0,    0,    0,   21,    0, -152,
 -152, -239, -249,    0,    0,    0,    0,    0,    0,   -6,
    0,    0, -206,   17,    0,    0,    0, -152, -152, -152,
 -152, -152, -152,  -91,    0,    0,    0,  -68,  -22,    5,
   19,    0,    0,  -14,    0,    0,   21,    0, -231,    0,
 -124, -176, -231, -152, -151,    0,    0,    0,    0,    0,
    0,    0,  -45,    0, -152, -152, -152, -152, -152, -152,
 -152, -152,    0,    0,    0,    0,    0, -114,    0,    0,
    0,    0,    0,    0, -192,    0,    0, -251,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -124,    0,    0,
};
short yygindex[] = {                                     89,
    0,  174,   87,  -42,   30,    0,   67,   74,   40,   -5,
    0,    0,    0,  -10,    2,   16,   -1,    0,  105,  112,
   -4,    0,  268,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 329
short yytable[] = {                                      35,
    1,    9,   20,   27,   82,   10,    9,   48,    5,   41,
   10,    5,   28,    1,   45,    7,   29,   49,    5,   26,
   30,    5,   51,   24,   31,   32,    5,   61,   63,    5,
   60,    1,   26,   33,   14,   99,    1,   52,   27,   14,
   85,   20,   27,   20,   27,   80,   27,   88,   27,   49,
    9,   53,   16,   64,   10,   92,   93,   94,   95,   96,
   97,   98,   24,   91,   24,   54,   78,  102,   24,   20,
   65,   81,   20,   27,   11,  101,   12,   17,   13,   20,
    1,    5,   79,   14,    5,   35,   46,   35,   55,    7,
   85,    6,   35,   24,   35,   35,   35,   20,   35,   35,
   35,   47,   35,   35,   35,    5,    5,   35,    5,    5,
   34,  100,   34,   35,   35,   83,   35,   34,    9,   34,
   34,   34,   10,   34,   34,   34,   87,   34,   34,   34,
    9,    9,   34,    5,   10,   10,    5,   19,   34,   34,
  103,   34,   32,    5,   62,    0,    5,   19,    1,    0,
   33,   14,    0,    0,   32,   11,   86,    8,   89,   90,
    1,    1,    0,   14,   14,   19,    0,    0,    0,   16,
   41,    0,   41,    4,    5,    8,    4,   41,    0,   41,
   41,   41,    0,   41,   41,   41,    0,   41,   41,   41,
    0,    0,    0,   40,    0,   40,    0,    0,   41,    0,
   40,   41,   40,   40,   40,    0,   40,   40,   40,    0,
   40,   40,   40,    0,    0,    0,   32,    0,   32,    0,
    0,   40,    0,   32,   40,   32,   32,   32,    0,   32,
   32,   32,    0,   32,   32,   32,    0,    0,    0,   48,
    0,   48,    0,    5,   32,    0,    5,   32,   48,   48,
   48,    0,   48,   48,   48,   13,   48,   48,   48,    0,
    0,    5,    0,    0,   26,   13,   60,   48,   60,    0,
   48,   27,    0,   27,    0,   60,   60,   26,    5,    0,
    0,    5,    8,   13,   65,   60,   30,   24,    0,   25,
    0,    0,    8,    0,   60,   56,   57,   58,   59,    0,
   24,    0,   25,    0,   16,    0,    0,   65,   66,   30,
   67,   68,   69,    0,   70,   71,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   72,
};
short yycheck[] = {                                      10,
    0,  259,    8,    8,   47,  263,  259,  262,  258,   11,
  263,  261,  270,  289,   13,  261,  274,  272,  258,  271,
  278,  261,  284,    8,  282,  283,  258,   33,   33,  261,
   32,  289,  284,  291,  292,   78,  289,  291,  288,  292,
   51,   47,   47,   49,   49,   44,  286,   53,   53,  272,
  259,  271,  284,  264,  263,   66,   67,   68,   69,   70,
   71,   72,   47,   65,   49,  268,  281,  290,   53,  262,
  269,  288,   78,   78,  283,  271,  285,  284,  287,  272,
  289,  258,  286,  292,  261,  262,  258,  264,  291,  261,
  101,    3,  269,   78,  271,  272,  273,  290,  275,  276,
  277,   15,  279,  280,  281,  258,  258,  284,  261,  261,
  262,   82,  264,  290,  291,   49,  293,  269,  259,  271,
  272,  273,  263,  275,  276,  277,   53,  279,  280,  281,
  259,  259,  284,  258,  263,  263,  261,  262,  290,  291,
  101,  293,  283,  258,   33,   -1,  261,  272,  289,   -1,
  291,  292,   -1,   -1,  283,  283,   52,  272,   54,   55,
  289,  289,   -1,  292,  292,  290,   -1,   -1,   -1,  284,
  262,   -1,  264,    0,    1,  290,    3,  269,   -1,  271,
  272,  273,   -1,  275,  276,  277,   -1,  279,  280,  281,
   -1,   -1,   -1,  262,   -1,  264,   -1,   -1,  290,   -1,
  269,  293,  271,  272,  273,   -1,  275,  276,  277,   -1,
  279,  280,  281,   -1,   -1,   -1,  262,   -1,  264,   -1,
   -1,  290,   -1,  269,  293,  271,  272,  273,   -1,  275,
  276,  277,   -1,  279,  280,  281,   -1,   -1,   -1,  262,
   -1,  264,   -1,  258,  290,   -1,  261,  293,  271,  272,
  273,   -1,  275,  276,  277,  262,  279,  280,  281,   -1,
   -1,  261,   -1,   -1,  271,  272,  262,  290,  264,   -1,
  293,  286,   -1,  288,   -1,  271,  272,  284,  258,   -1,
   -1,  261,  262,  290,  268,  281,  268,  271,   -1,  271,
   -1,   -1,  272,   -1,  290,   28,   29,   30,   31,   -1,
  284,   -1,  284,   -1,  284,   -1,   -1,  291,  273,  291,
  275,  276,  277,   -1,  279,  280,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 293
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOKEN_EOF","TOKEN_WORD",
"TOKEN_DECIMAL_INTEGER","TOKEN_SCHEME_SCRIPT","TOKEN_OPEN_BLOCK",
"TOKEN_CLOSE_BLOCK","TOKEN_OPEN_PAREN","TOKEN_CLOSE_PAREN","TOKEN_OPEN_BRACKET",
"TOKEN_CLOSE_BRACKET","TOKEN_DOT","TOKEN_COLON","TOKEN_BACKSLASH","TOKEN_TILDE",
"TOKEN_COMMA","TOKEN_SEMICOLON","TOKEN_ADD","TOKEN_SUBTRACT","TOKEN_MULTIPLY",
"TOKEN_DIVIDE","TOKEN_MODULO","TOKEN_NOT","TOKEN_OR","TOKEN_AND","TOKEN_GUARD",
"TOKEN_REFERENCE","TOKEN_DEREFERENCE","TOKEN_ASSIGN","TOKEN_IF","TOKEN_FI",
"TOKEN_DO","TOKEN_OD","TOKEN_SCHEME","TOKEN_END_GUARD",
"TOKEN_FUNCTION_APPLICATION","TOKEN_CONSTANT","TOKEN_TEST_EQUALITY",
};
char *yyrule[] = {
"$accept : function_collection",
"function_collection :",
"function_collection : exterior_function_constant function_collection",
"exterior_function_constant : scheme begin_function command_list end_function",
"function_constant : scheme begin_function command_list end_function",
"scheme :",
"scheme : TOKEN_SCHEME scheme",
"begin_function : TOKEN_OPEN_BLOCK",
"command_list :",
"command_list : command",
"command_list : command_list TOKEN_SEMICOLON command",
"end_function : TOKEN_CLOSE_BLOCK",
"command : lvalue_list TOKEN_ASSIGN expression_list",
"command : terminated_combinator_expression",
"command : if_statement",
"command : do_statement",
"lvalue_list :",
"lvalue_list : lvalue",
"lvalue_list : lvalue TOKEN_COMMA lvalue_list",
"expression_list :",
"expression_list : expression",
"expression_list : expression TOKEN_COMMA expression_list",
"if_statement : TOKEN_IF guarded_command_list TOKEN_FI",
"do_statement : TOKEN_DO guarded_command_list TOKEN_OD",
"lvalue : scheme_name",
"lvalue : TOKEN_DEREFERENCE atomic_expression",
"lvalue : terminated_combinator_expression",
"guarded_command_list :",
"guarded_command_list : guarded_command guarded_command_list",
"guarded_command : expression TOKEN_GUARD command_list TOKEN_END_GUARD",
"tier0_expression : TOKEN_DEREFERENCE atomic_expression",
"tier0_expression : atomic_expression",
"uninitiated_combinator_expression : TOKEN_FUNCTION_APPLICATION proper_combinator_expression",
"uninitiated_combinator_expression : TOKEN_FUNCTION_APPLICATION uninitiated_combinator_expression",
"terminated_combinator_expression : proper_combinator_expression TOKEN_FUNCTION_APPLICATION",
"terminated_combinator_expression : terminated_combinator_expression TOKEN_FUNCTION_APPLICATION",
"proper_combinator_expression : tier0_expression",
"proper_combinator_expression : proper_combinator_expression TOKEN_FUNCTION_APPLICATION tier0_expression",
"proper_combinator_expression : terminated_combinator_expression TOKEN_FUNCTION_APPLICATION tier0_expression",
"proper_combinator_expression : proper_combinator_expression TOKEN_COLON tier0_expression",
"functional_expression : proper_combinator_expression",
"functional_expression : terminated_combinator_expression",
"functional_expression : uninitiated_combinator_expression",
"functional_expression : functional_expression TOKEN_BACKSLASH atomic_expression",
"tier1_expression : TOKEN_REFERENCE tier1_expression",
"tier1_expression : TOKEN_NOT tier1_expression",
"tier1_expression : TOKEN_SUBTRACT tier1_expression",
"tier1_expression : TOKEN_TILDE tier1_expression",
"tier1_expression : functional_expression",
"tier2_expression : TOKEN_ADD expression",
"tier3_expression : TOKEN_MULTIPLY expression",
"tier3_expression : TOKEN_DIVIDE expression",
"tier3_expression : TOKEN_MODULO expression",
"tier3_expression : tier2_expression",
"tier3_point_5_expression : TOKEN_TEST_EQUALITY expression",
"tier3_point_5_expression : tier3_expression",
"tier4_expression : TOKEN_AND expression",
"tier4_expression : tier3_point_5_expression",
"tier5_expression : TOKEN_OR expression",
"tier5_expression : tier4_expression",
"expression : tier1_expression",
"expression : tier1_expression tier5_expression",
"atomic_expression : TOKEN_DECIMAL_INTEGER",
"atomic_expression : TOKEN_CONSTANT",
"atomic_expression : function_constant",
"atomic_expression : scheme_name",
"atomic_expression : TOKEN_OPEN_PAREN expression TOKEN_CLOSE_PAREN",
"scheme_name : scheme TOKEN_WORD",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
int yyparse __P((void));
static int yygrowstack __P((void));
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    if ((newss = (short *)realloc(yyss, newsize * sizeof *newss)) == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    if ((newvs = (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs)) == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    int yym, yyn, yystate;
#if YYDEBUG
    char *yys;

    if ((yys = getenv("YYDEBUG")) != NULL)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
    goto yynewerror;
yynewerror:
    yyerror("syntax error");
    goto yyerrlab;
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 134 "parser.y"
{
  xscheme_complete();
}
break;
case 2:
#line 139 "parser.y"
{
  execute_tree( yyvsp[-1] );
}
break;
case 3:
#line 145 "parser.y"
{
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  char buffer[5];
  sprintf(buffer, "%d", current_label );
  yyval = create_tree( end_code, current_label, create_tree(begin_code, 0, 0, 0), yyvsp[-1] );
  current_label = current_label+1;
  xscheme_leaf("label", buffer);
  xscheme_node( "block", 2 ); 
  xscheme_eval( yyvsp[-3] );
}
break;
case 4:
#line 163 "parser.y"
{
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  yyval = create_tree( dump, "CHECK\nHALT\nPOP-CODE\n", create_tree(dump, "PUSH\nPUSH-CODE\n", 0, 0), yyvsp[-1] );
  xscheme_node( "block", 1 ); 
  xscheme_eval( yyvsp[-3] );
}
break;
case 5:
#line 176 "parser.y"
{ yyval = 0; }
break;
case 6:
#line 179 "parser.y"
{yyval = xscheme_push(); }
break;
case 7:
#line 184 "parser.y"
{

}
break;
case 8:
#line 190 "parser.y"
{
  xscheme_ground("command-list");
  yyval = 0;
}
break;
case 9:
#line 196 "parser.y"
{ 
   xscheme_ground("command-list");
   xscheme_list("command-list");
   yyval = yyvsp[0];
}
break;
case 10:
#line 203 "parser.y"
{
  xscheme_list("command-list");
  yyval = create_tree( nil, 0, yyvsp[-2], yyvsp[0] );
}
break;
case 11:
#line 211 "parser.y"
{
}
break;
case 12:
#line 217 "parser.y"
{
  xscheme_node("assignment", 2);
  yyval = create_tree( dump, "ASSIGN\n", yyvsp[-2], yyvsp[0] );
}
break;
case 13:
#line 223 "parser.y"
{
  xscheme_node("function-call", 1);
  yyval = create_tree( dump, "CHECK\n", yyvsp[0], 0 );
}
break;
case 14:
#line 229 "parser.y"
{
  xscheme_node("command", 0);
  yyval = yyvsp[0];
}
break;
case 15:
#line 235 "parser.y"
{
  xscheme_node("command", 0);
  yyval = yyvsp[0];
}
break;
case 16:
#line 242 "parser.y"
{
  xscheme_ground("lvalue-list");
  yyval = create_tree(dump, "ANONYMOUS-RETURN\n", 0, 0);
}
break;
case 17:
#line 248 "parser.y"
{
  xscheme_ground("lvalue-list");
  xscheme_list("lvalue-list");
  yyval = yyvsp[0];
}
break;
case 18:
#line 255 "parser.y"
{
  xscheme_list("lvalue-list");
  yyval = create_tree( nil, 0, yyvsp[-2], yyvsp[0] );
}
break;
case 19:
#line 262 "parser.y"
{
  xscheme_ground("expression-list");
  yyval = create_tree(dump, "ANONYMOUS-PARAMETER\n", 0, 0);
}
break;
case 20:
#line 268 "parser.y"
{
  xscheme_ground("expression-list");
  xscheme_list("expression-list");
  yyval = yyvsp[0];
}
break;
case 21:
#line 275 "parser.y"
{
  xscheme_list("expression-list");
  yyval = create_tree( dump, "EXPRESSION LIST\n", yyvsp[-2], yyvsp[0] );
  if( yyvsp[-2] == 0 || yyvsp[0] == 0 ) { printf("one of the list items is 0\n"); }
}
break;
case 22:
#line 284 "parser.y"
{
  yyval = create_tree( nil, 0, yyvsp[-2], 0 );
  xscheme_node("if", 1 );
}
break;
case 23:
#line 292 "parser.y"
{
  yyval = create_tree( nil, 0, yyvsp[-2], 0 );
  xscheme_node( "do", 1 );
}
break;
case 24:
#line 300 "parser.y"
{
  xscheme_node("lvalue-name", 1);
  yyval = create_tree( nil, 0, create_tree( dump, "LVALUE-NAME\n", 0, 0 ), yyvsp[0] );
}
break;
case 25:
#line 306 "parser.y"
{
  printf("dereference\n");
  xscheme_node("lvalue-address", 0);
  yyval = create_tree( dump, (int)"LVALUE-ADDRESS\n", yyvsp[0], 0 );
}
break;
case 26:
#line 313 "parser.y"
{
  xscheme_node("lvalue-call", 1);
  yyval = create_tree( dump, (int)"LVALUE-CALL\n", yyvsp[0], 0 );
}
break;
case 27:
#line 320 "parser.y"
{
  xscheme_ground("guarded-command-list");
}
break;
case 28:
#line 325 "parser.y"
{
  xscheme_list("guarded_command_list");
}
break;
case 29:
#line 332 "parser.y"
{
  create_tree( nil, 0, yyvsp[-3], yyvsp[-1] );
  xscheme_node("guarded-command", 2);
}
break;
case 30:
#line 340 "parser.y"
{
  printf("dereference\n");
}
break;
case 31:
#line 345 "parser.y"
{
  yyval = yyvsp[0];
}
break;
case 32:
#line 352 "parser.y"
{
}
break;
case 33:
#line 356 "parser.y"
{
}
break;
case 34:
#line 362 "parser.y"
{
yyval = create_tree( dump, "CURRY\n", yyvsp[-1], 0 );
xscheme_node("curry", 1);
}
break;
case 35:
#line 368 "parser.y"
{
yyval = create_tree( dump, "CURRY\n", yyvsp[-1], 0 );
xscheme_node("curry", 1);
}
break;
case 36:
#line 376 "parser.y"
{
yyval = yyvsp[0];
}
break;
case 37:
#line 381 "parser.y"
{
yyval = create_tree( dump, "APPLY\n", yyvsp[-2], yyvsp[0] );
xscheme_node("apply", 2 );
}
break;
case 38:
#line 387 "parser.y"
{
yyval = create_tree( dump, "APPLY\n", yyvsp[-2], yyvsp[0] );
 xscheme_node("apply", 2 );
}
break;
case 39:
#line 393 "parser.y"
{
  yyval = create_tree( dump, "NAMESPACE-JOIN\n", yyvsp[-2], yyvsp[0]);
  xscheme_node("namespace-join", 2);
}
break;
case 40:
#line 401 "parser.y"
{
yyval = yyvsp[0];
}
break;
case 41:
#line 406 "parser.y"
{
yyval = yyvsp[0];
}
break;
case 42:
#line 411 "parser.y"
{
}
break;
case 43:
#line 415 "parser.y"
{
  yyval = create_tree( dump, "LEFT-JOIN\n", yyvsp[-2], yyvsp[0] );
  xscheme_node( "left-join", 2 );
}
break;
case 44:
#line 423 "parser.y"
{
  printf("reference\n");
}
break;
case 45:
#line 428 "parser.y"
{
  printf("not\n");
}
break;
case 46:
#line 433 "parser.y"
{
  printf("negate\n");
}
break;
case 47:
#line 438 "parser.y"
{
  yyval = create_tree( dump, "SPAWN\n", yyvsp[0], 0 );
  xscheme_node( "spawn", 1 );
}
break;
case 48:
#line 444 "parser.y"
{
  yyval = create_tree( dump, "CHECK\n", yyvsp[0], 0 );
}
break;
case 49:
#line 451 "parser.y"
{
  yyval = create_tree(dump, "ADD\n", yyvsp[0], 0 );
  xscheme_node ( "+", 2 );
}
break;
case 50:
#line 459 "parser.y"
{
  printf("multiply\n");
  xscheme_node( "*", 2 );
}
break;
case 51:
#line 465 "parser.y"
{
  xscheme_node( "/", 2 );
}
break;
case 52:
#line 470 "parser.y"
{
  xscheme_node( "%", 2 );
}
break;
case 53:
#line 475 "parser.y"
{
   yyval = yyvsp[0];
}
break;
case 54:
#line 482 "parser.y"
{
  yyval = create_tree( dump, "EQUAL\n", yyvsp[0], 0 ); 
  xscheme_node( "eq?", 2 );
}
break;
case 55:
#line 488 "parser.y"
{
  yyval = yyvsp[0];
}
break;
case 56:
#line 494 "parser.y"
{
  xscheme_node( "and", 2 );
}
break;
case 57:
#line 499 "parser.y"
{
  yyval = yyvsp[0];
}
break;
case 58:
#line 506 "parser.y"
{
  xscheme_node("or", 2);
}
break;
case 59:
#line 511 "parser.y"
{
  yyval = yyvsp[0];
}
break;
case 60:
#line 518 "parser.y"
{
  yyval = yyvsp[0];
  xscheme_node("expression", 1);
}
break;
case 61:
#line 524 "parser.y"
{
  xscheme_node("expression", 1);
  yyval = create_tree( nil, 0, yyvsp[-1], yyvsp[0]);
}
break;
case 62:
#line 532 "parser.y"
{
    int n;
    sscanf( yytext, "%d", &n );
    yyval = create_tree( push_literal, n , 0, 0 );
    xscheme_leaf("constant", yytext);
  }
break;
case 63:
#line 540 "parser.y"
{
    if( !const_size )
      {
      yyval = create_tree( literal, constant, 0, 0 );
      } else {
      yyval = create_tree( constant_block, create_constant_block(const_size, const_buffer), 0, 0 );
      }
    xscheme_node("const", 2);
  }
break;
case 64:
#line 551 "parser.y"
{
  yyval = yyvsp[0];
}
break;
case 65:
#line 556 "parser.y"
{
    yyval = create_tree( nil, 0, create_tree( dump, "REFERENCE-VARIABLE\n", 0, 0 ), yyvsp[0] );
  }
break;
case 66:
#line 561 "parser.y"
{
  yyval = yyvsp[-1];
}
break;
case 67:
#line 568 "parser.y"
{
   char offset_s[10];
   int offset_d = get_name(yytext);
   sprintf(offset_s, "%d", offset_d);
   yyval = create_tree(literal, offset_d, 0, 0);
   xscheme_leaf("name", yytext);
   xscheme_leaf("offset", offset_s);
   xscheme_node("identifier", 2);
   xscheme_eval( yyvsp[-1] );
}
break;
#line 1035 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
