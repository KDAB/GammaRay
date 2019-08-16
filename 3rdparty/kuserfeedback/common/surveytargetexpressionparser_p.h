/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SURVEYTARGETEXPRESSIONPARSER_P_H_INCLUDED
# define YY_YY_SURVEYTARGETEXPRESSIONPARSER_P_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 42 "surveytargetexpressionparser.y"


#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif


#line 57 "surveytargetexpressionparser_p.h"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_OP_OR = 258,
    T_OP_AND = 259,
    T_OP_EQ = 260,
    T_OP_NEQ = 261,
    T_OP_GT = 262,
    T_OP_GE = 263,
    T_OP_LT = 264,
    T_OP_LE = 265,
    T_DOT = 266,
    T_LPAREN = 267,
    T_RPAREN = 268,
    T_LBRACKET = 269,
    T_RBRACKET = 270,
    T_DOUBLE = 271,
    T_INTEGER = 272,
    T_BOOLEAN = 273,
    T_STRING = 274,
    T_IDENTIFIER = 275
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 56 "surveytargetexpressionparser.y"

    int intVal;
    double doubleVal;
    bool boolVal;
    char *str;
    KUserFeedback::SurveyTargetExpression *expression;

#line 97 "surveytargetexpressionparser_p.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (KUserFeedback::SurveyTargetExpression **expression, yyscan_t scanner);

#endif /* !YY_YY_SURVEYTARGETEXPRESSIONPARSER_P_H_INCLUDED  */
