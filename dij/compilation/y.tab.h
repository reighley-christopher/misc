
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

extern YYSTYPE yylval;


