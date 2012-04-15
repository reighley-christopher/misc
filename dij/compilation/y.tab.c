
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

#include <stdlib.h>
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


/* Line 189 of yacc.c  */
#line 164 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 258,
     TOKEN_WORD = 259,
     TOKEN_DECIMAL_INTEGER = 260,
     TOKEN_SCHEME_SCRIPT = 261,
     TOKEN_OPEN_BLOCK = 262,
     TOKEN_CLOSE_BLOCK = 263,
     TOKEN_OPEN_PAREN = 264,
     TOKEN_CLOSE_PAREN = 265,
     TOKEN_OPEN_BRACKET = 266,
     TOKEN_CLOSE_BRACKET = 267,
     TOKEN_DOT = 268,
     TOKEN_COLON = 269,
     TOKEN_BACKSLASH = 270,
     TOKEN_TILDE = 271,
     TOKEN_COMMA = 272,
     TOKEN_SEMICOLON = 273,
     TOKEN_ADD = 274,
     TOKEN_SUBTRACT = 275,
     TOKEN_MULTIPLY = 276,
     TOKEN_DIVIDE = 277,
     TOKEN_MODULO = 278,
     TOKEN_NOT = 279,
     TOKEN_OR = 280,
     TOKEN_AND = 281,
     TOKEN_GUARD = 282,
     TOKEN_REFERENCE = 283,
     TOKEN_DEREFERENCE = 284,
     TOKEN_ASSIGN = 285,
     TOKEN_IF = 286,
     TOKEN_FI = 287,
     TOKEN_DO = 288,
     TOKEN_OD = 289,
     TOKEN_SCHEME = 290,
     TOKEN_END_GUARD = 291,
     TOKEN_FUNCTION_APPLICATION = 292,
     TOKEN_CONSTANT = 293,
     TOKEN_TEST_EQUALITY = 294
   };
#endif
/* Tokens.  */
#define TOKEN_EOF 258
#define TOKEN_WORD 259
#define TOKEN_DECIMAL_INTEGER 260
#define TOKEN_SCHEME_SCRIPT 261
#define TOKEN_OPEN_BLOCK 262
#define TOKEN_CLOSE_BLOCK 263
#define TOKEN_OPEN_PAREN 264
#define TOKEN_CLOSE_PAREN 265
#define TOKEN_OPEN_BRACKET 266
#define TOKEN_CLOSE_BRACKET 267
#define TOKEN_DOT 268
#define TOKEN_COLON 269
#define TOKEN_BACKSLASH 270
#define TOKEN_TILDE 271
#define TOKEN_COMMA 272
#define TOKEN_SEMICOLON 273
#define TOKEN_ADD 274
#define TOKEN_SUBTRACT 275
#define TOKEN_MULTIPLY 276
#define TOKEN_DIVIDE 277
#define TOKEN_MODULO 278
#define TOKEN_NOT 279
#define TOKEN_OR 280
#define TOKEN_AND 281
#define TOKEN_GUARD 282
#define TOKEN_REFERENCE 283
#define TOKEN_DEREFERENCE 284
#define TOKEN_ASSIGN 285
#define TOKEN_IF 286
#define TOKEN_FI 287
#define TOKEN_DO 288
#define TOKEN_OD 289
#define TOKEN_SCHEME 290
#define TOKEN_END_GUARD 291
#define TOKEN_FUNCTION_APPLICATION 292
#define TOKEN_CONSTANT 293
#define TOKEN_TEST_EQUALITY 294




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 284 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   248

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  69
/* YYNRULES -- Number of states.  */
#define YYNSTATES  106

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,    12,    17,    18,    21,    23,
      24,    26,    30,    32,    36,    38,    40,    42,    43,    45,
      49,    50,    52,    56,    60,    64,    66,    69,    71,    72,
      75,    80,    83,    85,    88,    91,    94,    97,    99,   103,
     107,   111,   113,   115,   117,   121,   124,   127,   130,   133,
     135,   138,   141,   144,   147,   149,   152,   154,   157,   159,
     162,   164,   166,   169,   171,   173,   175,   177,   181,   183
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      41,     0,    -1,    -1,    42,    41,    -1,    44,    45,    46,
      47,    -1,    44,    45,    46,    47,    -1,    -1,    35,    44,
      -1,     7,    -1,    -1,    48,    -1,    46,    18,    48,    -1,
       8,    -1,    49,    30,    50,    -1,    58,    -1,    51,    -1,
      52,    -1,    -1,    53,    -1,    53,    17,    49,    -1,    -1,
      67,    -1,    67,    17,    50,    -1,    31,    54,    32,    -1,
      33,    54,    34,    -1,    70,    -1,    29,    68,    -1,    58,
      -1,    -1,    55,    54,    -1,    67,    27,    46,    36,    -1,
      29,    68,    -1,    68,    -1,    37,    59,    -1,    37,    57,
      -1,    59,    37,    -1,    58,    37,    -1,    56,    -1,    59,
      37,    56,    -1,    58,    37,    56,    -1,    59,    14,    56,
      -1,    59,    -1,    58,    -1,    57,    -1,    60,    15,    68,
      -1,    28,    61,    -1,    24,    61,    -1,    20,    61,    -1,
      16,    61,    -1,    60,    -1,    19,    67,    -1,    21,    67,
      -1,    22,    67,    -1,    23,    67,    -1,    62,    -1,    39,
      67,    -1,    63,    -1,    26,    67,    -1,    64,    -1,    25,
      67,    -1,    65,    -1,    61,    -1,    61,    66,    -1,     5,
      -1,    38,    -1,    43,    -1,    70,    -1,     9,    67,    10,
      -1,     4,    -1,    44,    69,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   134,   134,   138,   144,   162,   177,   179,   184,   191,
     196,   203,   211,   217,   223,   229,   235,   243,   248,   255,
     263,   268,   275,   284,   292,   300,   306,   313,   321,   325,
     332,   340,   345,   352,   356,   362,   368,   376,   381,   387,
     393,   401,   406,   411,   415,   423,   428,   433,   438,   444,
     451,   459,   465,   470,   475,   482,   488,   494,   499,   506,
     511,   518,   524,   532,   540,   551,   556,   561,   568,   581
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_EOF", "TOKEN_WORD",
  "TOKEN_DECIMAL_INTEGER", "TOKEN_SCHEME_SCRIPT", "TOKEN_OPEN_BLOCK",
  "TOKEN_CLOSE_BLOCK", "TOKEN_OPEN_PAREN", "TOKEN_CLOSE_PAREN",
  "TOKEN_OPEN_BRACKET", "TOKEN_CLOSE_BRACKET", "TOKEN_DOT", "TOKEN_COLON",
  "TOKEN_BACKSLASH", "TOKEN_TILDE", "TOKEN_COMMA", "TOKEN_SEMICOLON",
  "TOKEN_ADD", "TOKEN_SUBTRACT", "TOKEN_MULTIPLY", "TOKEN_DIVIDE",
  "TOKEN_MODULO", "TOKEN_NOT", "TOKEN_OR", "TOKEN_AND", "TOKEN_GUARD",
  "TOKEN_REFERENCE", "TOKEN_DEREFERENCE", "TOKEN_ASSIGN", "TOKEN_IF",
  "TOKEN_FI", "TOKEN_DO", "TOKEN_OD", "TOKEN_SCHEME", "TOKEN_END_GUARD",
  "TOKEN_FUNCTION_APPLICATION", "TOKEN_CONSTANT", "TOKEN_TEST_EQUALITY",
  "$accept", "function_collection", "exterior_function_constant",
  "function_constant", "scheme", "begin_function", "command_list",
  "end_function", "command", "lvalue_list", "expression_list",
  "if_statement", "do_statement", "lvalue", "guarded_command_list",
  "guarded_command", "tier0_expression",
  "uninitiated_combinator_expression", "terminated_combinator_expression",
  "proper_combinator_expression", "functional_expression",
  "tier1_expression", "tier2_expression", "tier3_expression",
  "tier3_point_5_expression", "tier4_expression", "tier5_expression",
  "expression", "atomic_expression", "name", "scheme_name", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    41,    42,    43,    44,    44,    45,    46,
      46,    46,    47,    48,    48,    48,    48,    49,    49,    49,
      50,    50,    50,    51,    52,    53,    53,    53,    54,    54,
      55,    56,    56,    57,    57,    58,    58,    59,    59,    59,
      59,    60,    60,    60,    60,    61,    61,    61,    61,    61,
      62,    63,    63,    63,    63,    64,    64,    65,    65,    66,
      66,    67,    67,    68,    68,    68,    68,    68,    69,    70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     4,     4,     0,     2,     1,     0,
       1,     3,     1,     3,     1,     1,     1,     0,     1,     3,
       0,     1,     3,     3,     3,     1,     2,     1,     0,     2,
       4,     2,     1,     2,     2,     2,     2,     1,     3,     3,
       3,     1,     1,     1,     3,     2,     2,     2,     2,     1,
       2,     2,     2,     2,     1,     2,     1,     2,     1,     2,
       1,     1,     2,     1,     1,     1,     1,     3,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     6,     0,     2,     0,     7,     1,     3,     8,     6,
      63,     6,     6,     6,     6,    64,    65,     0,     0,    10,
       0,    15,    16,    18,    37,    14,     0,    32,    25,     6,
       6,     6,     6,     6,     6,    43,    42,    41,    49,    61,
       0,    66,    26,     0,     6,     0,     0,    68,     6,    69,
      12,     6,     4,    20,     6,    36,     6,    35,    48,    47,
      46,    45,    31,    34,     0,    33,     6,     6,     6,     6,
       6,     6,     6,     6,    54,    56,    58,    60,    62,    67,
      23,    29,     6,    24,     0,    11,    13,    21,    19,    27,
      39,    40,    38,    44,    50,    51,    52,    53,    59,    57,
      55,     0,     5,    20,    30,    22
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,    16,    17,     9,    18,    52,    19,    20,
      86,    21,    22,    23,    43,    44,    24,    35,    36,    37,
      38,    39,    74,    75,    76,    77,    78,    45,    27,    49,
      41
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -39
static const yytype_int16 yypact[] =
{
       1,   -10,    33,     1,    27,   -39,   -39,   -39,   -39,   117,
     -39,   156,    45,   104,   129,   -39,   -39,    65,     5,   -39,
       9,   -39,   -39,    50,   -39,    54,    16,   -39,    18,   156,
     156,   156,   156,    45,   183,   -39,    36,    16,    62,   203,
      66,   -39,    49,    53,    73,    60,    47,   -39,   117,   -39,
     -39,   178,   -39,     0,   205,    61,   210,    61,   -39,   -39,
     -39,   -39,   -39,   -39,    36,    16,    45,   156,   156,   156,
     156,   156,   156,   156,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,   -39,   168,   -39,     5,   -39,   -39,    71,   -39,    36,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,   -15,   -39,     0,   -39,   -39
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -39,    91,   -39,   -39,    11,    78,   -33,    14,    52,    46,
       3,   -39,   -39,   -39,     8,   -39,   -38,    70,    -8,    -7,
     -39,    85,   -39,   -39,   -39,   -39,   -39,   -11,    -2,   -39,
      -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -67
static const yytype_int8 yytable[] =
{
      40,    25,    26,    51,    -6,    10,    28,    -6,    -6,    11,
      42,     4,     5,    50,     4,    84,    29,    90,    91,    92,
      30,   104,    46,    51,    31,     1,    64,    65,    32,    33,
      56,    62,   -66,     6,     8,     1,     1,    34,    15,    53,
      25,    26,    87,    25,    26,    28,    89,    26,    28,   101,
      10,    28,    81,    57,    11,   -66,    94,    95,    96,    97,
      98,    99,   100,   -31,    93,    -6,    10,    54,    -6,    47,
      11,   -27,     8,    55,    25,    26,    79,    66,    10,    28,
       1,    83,    11,    15,   -27,    80,   -31,    82,   103,    29,
      33,    55,    87,    30,     7,    48,     1,    31,   102,    15,
      88,    32,    33,    85,    63,   -28,   105,   -28,     1,    10,
      34,    15,     0,    11,    58,    59,    60,    61,     0,     0,
      29,     0,    10,     0,    30,    -9,    11,     0,    31,     0,
       0,     0,    32,    33,    10,    -9,   -28,     0,    11,     1,
       0,    34,    15,     0,     0,    29,    12,   -17,    13,    30,
      14,     0,     1,    31,     0,    15,     0,    32,    33,     0,
       0,    10,     0,   -28,     1,    11,    34,    15,     0,     0,
       0,     0,    29,    10,     0,     0,    30,    11,     0,     0,
      31,     0,     0,    10,    32,    33,    -9,    11,    10,     0,
       0,     1,    11,    34,    15,     0,     0,    12,   -17,    13,
       0,    14,     0,     1,    -9,     0,    15,    12,   -17,    13,
      10,    14,    33,     1,    11,    10,    15,     0,     1,    11,
      34,    15,    67,     0,    68,    69,    70,     0,    71,    72,
       0,     0,     0,     0,    12,   -17,     0,     0,     0,    33,
       1,     0,    73,    15,     0,     1,     0,     0,    15
};

static const yytype_int8 yycheck[] =
{
      11,     9,     9,    18,     4,     5,     9,     7,     7,     9,
      12,     0,     1,     8,     3,    48,    16,    55,    56,    57,
      20,    36,    14,    18,    24,    35,    34,    34,    28,    29,
      14,    33,    14,     0,     7,    35,    35,    37,    38,    30,
      48,    48,    53,    51,    51,    48,    54,    54,    51,    82,
       5,    54,    44,    37,     9,    37,    67,    68,    69,    70,
      71,    72,    73,    14,    66,     4,     5,    17,     7,     4,
       9,    17,     7,    37,    82,    82,    10,    15,     5,    82,
      35,    34,     9,    38,    30,    32,    37,    27,    17,    16,
      29,    37,   103,    20,     3,    17,    35,    24,    84,    38,
      54,    28,    29,    51,    34,    32,   103,    34,    35,     5,
      37,    38,    -1,     9,    29,    30,    31,    32,    -1,    -1,
      16,    -1,     5,    -1,    20,     8,     9,    -1,    24,    -1,
      -1,    -1,    28,    29,     5,    18,    32,    -1,     9,    35,
      -1,    37,    38,    -1,    -1,    16,    29,    30,    31,    20,
      33,    -1,    35,    24,    -1,    38,    -1,    28,    29,    -1,
      -1,     5,    -1,    34,    35,     9,    37,    38,    -1,    -1,
      -1,    -1,    16,     5,    -1,    -1,    20,     9,    -1,    -1,
      24,    -1,    -1,     5,    28,    29,    18,     9,     5,    -1,
      -1,    35,     9,    37,    38,    -1,    -1,    29,    30,    31,
      -1,    33,    -1,    35,    36,    -1,    38,    29,    30,    31,
       5,    33,    29,    35,     9,     5,    38,    -1,    35,     9,
      37,    38,    19,    -1,    21,    22,    23,    -1,    25,    26,
      -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    29,
      35,    -1,    39,    38,    -1,    35,    -1,    -1,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    35,    41,    42,    44,    44,     0,    41,     7,    45,
       5,     9,    29,    31,    33,    38,    43,    44,    46,    48,
      49,    51,    52,    53,    56,    58,    59,    68,    70,    16,
      20,    24,    28,    29,    37,    57,    58,    59,    60,    61,
      67,    70,    68,    54,    55,    67,    54,     4,    45,    69,
       8,    18,    47,    30,    17,    37,    14,    37,    61,    61,
      61,    61,    68,    57,    58,    59,    15,    19,    21,    22,
      23,    25,    26,    39,    62,    63,    64,    65,    66,    10,
      32,    54,    27,    34,    46,    48,    50,    67,    49,    58,
      56,    56,    56,    68,    67,    67,    67,    67,    67,    67,
      67,    46,    47,    17,    36,    50
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 134 "parser.y"
    {
  xscheme_complete();
}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 139 "parser.y"
    {
  execute_tree( (yyvsp[(1) - (2)]) );
}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 145 "parser.y"
    {
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  char buffer[5];
  sprintf(buffer, "%d", current_label );
  (yyval) = create_tree( end_code, current_label, create_tree(begin_code, 0, 0, 0), (yyvsp[(3) - (4)]) );
  current_label = current_label+1;
  xscheme_leaf("label", buffer);
  xscheme_node( "block", 2 ); 
  xscheme_eval( (char *)(yyvsp[(1) - (4)]) );
}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 163 "parser.y"
    {
  /*char seg[10];
  sprintf(seg, "%d", get_segment());
  empty_buffer( $3 );
  $$ = pop_code_scope();
  xscheme_leaf( "segment", seg); */
  (yyval) = create_tree( dump, (int)"CHECK\nHALT\nPOP-CODE\n", create_tree(dump, (int)"PUSH\nPUSH-CODE\n", 0, 0), (yyvsp[(3) - (4)]) );
  xscheme_node( "block", 1 ); 
//  xscheme_pop_scope();
  xscheme_eval( (char *)(yyvsp[(1) - (4)]) );
}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 177 "parser.y"
    { (yyval) = 0; }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 180 "parser.y"
    {(yyval) = (struct _tree *)xscheme_push(); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 185 "parser.y"
    {
//  xscheme_push_scope();
}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 191 "parser.y"
    {
  xscheme_ground("command-list");
  (yyval) = 0;
}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 197 "parser.y"
    { 
   xscheme_ground("command-list");
   xscheme_list("command-list");
   (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 204 "parser.y"
    {
  xscheme_list("command-list");
  (yyval) = create_tree( nil, 0, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 212 "parser.y"
    {
}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 218 "parser.y"
    {
  xscheme_node("assignment", 2);
  (yyval) = create_tree( dump, (int)"ASSIGN\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 224 "parser.y"
    {
  xscheme_node("function-call", 1);
  (yyval) = create_tree( dump, (int)"CHECK\n", (yyvsp[(1) - (1)]), 0 );
}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 230 "parser.y"
    {
  xscheme_node("command", 0);
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 236 "parser.y"
    {
  xscheme_node("command", 0);
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 243 "parser.y"
    {
  xscheme_ground("lvalue-list");
  (yyval) = create_tree(dump, (int)"ANONYMOUS-RETURN\n", 0, 0);
}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 249 "parser.y"
    {
  xscheme_ground("lvalue-list");
  xscheme_list("lvalue-list");
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 256 "parser.y"
    {
  xscheme_list("lvalue-list");
  (yyval) = create_tree( nil, 0, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 263 "parser.y"
    {
  xscheme_ground("expression-list");
  (yyval) = create_tree(dump, (int)"ANONYMOUS-PARAMETER\n", 0, 0);
}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 269 "parser.y"
    {
  xscheme_ground("expression-list");
  xscheme_list("expression-list");
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 276 "parser.y"
    {
  xscheme_list("expression-list");
  (yyval) = create_tree( dump, (int)"EXPRESSION LIST\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
  if( (yyvsp[(1) - (3)]) == 0 || (yyvsp[(3) - (3)]) == 0 ) { printf("one of the list items is 0\n"); }
}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 285 "parser.y"
    {
  (yyval) = create_tree( nil, 0, (yyvsp[(1) - (3)]), 0 );
  xscheme_node("if", 1 );
}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 293 "parser.y"
    {
  (yyval) = create_tree( nil, 0, (yyvsp[(1) - (3)]), 0 );
  xscheme_node( "do", 1 );
}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 301 "parser.y"
    {
  xscheme_node("lvalue-name", 1);
  (yyval) = create_tree( nil, 0, create_tree( dump, (int)"LVALUE-NAME\n", 0, 0 ), (yyvsp[(1) - (1)]) );
}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 307 "parser.y"
    {
  printf("dereference\n");
  xscheme_node("lvalue-address", 0);
  (yyval) = create_tree( dump, (int)"LVALUE-ADDRESS\n", (yyvsp[(2) - (2)]), 0 );
}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 314 "parser.y"
    {
  xscheme_node("lvalue-call", 1);
  (yyval) = create_tree( dump, (int)"LVALUE-CALL\n", (yyvsp[(1) - (1)]), 0 );
}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 321 "parser.y"
    {
  xscheme_ground("guarded-command-list");
}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 326 "parser.y"
    {
  xscheme_list("guarded_command_list");
}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 333 "parser.y"
    {
  create_tree( nil, 0, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]) );
  xscheme_node("guarded-command", 2);
}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 341 "parser.y"
    {
  printf("dereference\n");
}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 346 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 353 "parser.y"
    {
}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 357 "parser.y"
    {
}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 363 "parser.y"
    {
(yyval) = create_tree( dump, (int)"CURRY\n", (yyvsp[(1) - (2)]), 0 );
xscheme_node("curry", 1);
}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 369 "parser.y"
    {
(yyval) = create_tree( dump, (int)"CURRY\n", (yyvsp[(1) - (2)]), 0 );
xscheme_node("curry", 1);
}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 377 "parser.y"
    {
(yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 382 "parser.y"
    {
(yyval) = create_tree( dump, (int)"APPLY\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
xscheme_node("apply", 2 );
}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 388 "parser.y"
    {
(yyval) = create_tree( dump, (int)"APPLY\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
 xscheme_node("apply", 2 );
}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 394 "parser.y"
    {
  (yyval) = create_tree( dump, (int)"NAMESPACE-JOIN\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  xscheme_node("namespace-join", 2);
}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 402 "parser.y"
    {
(yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 407 "parser.y"
    {
(yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 412 "parser.y"
    {
}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 416 "parser.y"
    {
  (yyval) = create_tree( dump, (int)"LEFT-JOIN\n", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]) );
  xscheme_node( "left-join", 2 );
}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 424 "parser.y"
    {
  printf("reference\n");
}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 429 "parser.y"
    {
  printf("not\n");
}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 434 "parser.y"
    {
  printf("negate\n");
}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 439 "parser.y"
    {
  (yyval) = create_tree( dump, (int)"SPAWN\n", (yyvsp[(2) - (2)]), 0 );
  xscheme_node( "spawn", 1 );
}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 445 "parser.y"
    {
  (yyval) = create_tree( dump, (int)"CHECK\n", (yyvsp[(1) - (1)]), 0 );
}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 452 "parser.y"
    {
  (yyval) = create_tree(dump, (int)"ADD\n", (yyvsp[(2) - (2)]), 0 );
  xscheme_node ( "+", 2 );
}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 460 "parser.y"
    {
  printf("multiply\n");
  xscheme_node( "*", 2 );
}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 466 "parser.y"
    {
  xscheme_node( "/", 2 );
}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 471 "parser.y"
    {
  xscheme_node( "%", 2 );
}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 476 "parser.y"
    {
   (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 483 "parser.y"
    {
  (yyval) = create_tree( dump, (int)"EQUAL\n", (yyvsp[(2) - (2)]), 0 ); 
  xscheme_node( "eq?", 2 );
}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 489 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 495 "parser.y"
    {
  xscheme_node( "and", 2 );
}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 500 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 507 "parser.y"
    {
  xscheme_node("or", 2);
}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 512 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 519 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
  xscheme_node("expression", 1);
}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 525 "parser.y"
    {
  xscheme_node("expression", 1);
  (yyval) = create_tree( nil, 0, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]));
}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 533 "parser.y"
    {
    int n;
    sscanf( yytext, "%d", &n );
    (yyval) = create_tree( push_literal, n , 0, 0 );
    xscheme_leaf("constant", yytext);
  }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 541 "parser.y"
    {
    if( !const_size )
      {
      (yyval) = create_tree( literal, constant, 0, 0 );
      } else {
      (yyval) = create_tree( constant_block, (int)create_constant_block(const_size, const_buffer), 0, 0 );
      }
    xscheme_node("const", 2);
  }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 552 "parser.y"
    {
  (yyval) = (yyvsp[(1) - (1)]);
}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 557 "parser.y"
    {
    (yyval) = create_tree( nil, 0, create_tree( dump, (int)"REFERENCE-VARIABLE\n", 0, 0 ), (yyvsp[(1) - (1)]) );
  }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 562 "parser.y"
    {
  (yyval) = (yyvsp[(2) - (3)]);
}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 569 "parser.y"
    {
   char offset_s[10];
   int offset_d;
   offset_d = get_name(yytext);
   sprintf(offset_s, "%d", offset_d);
   (yyval) = create_tree(literal, offset_d, 0, 0);
   xscheme_leaf("name", yytext);
   xscheme_leaf("offset", offset_s);
   xscheme_node("identifier", 2);
}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 582 "parser.y"
    {
   (yyval) = (yyvsp[(2) - (2)]);
   xscheme_eval( (char *)(yyvsp[(1) - (2)]) );
}
    break;



/* Line 1455 of yacc.c  */
#line 2288 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



