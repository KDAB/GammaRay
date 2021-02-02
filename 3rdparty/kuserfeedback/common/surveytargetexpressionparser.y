%{
/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "surveytargetexpression.h"
#include "surveytargetexpressionparser_p.h"
#include "surveytargetexpressionscanner.h"

#include <stdio.h>

using namespace KUserFeedback;

void yyerror(KUserFeedback::SurveyTargetExpression **expression, yyscan_t scanner, char const* msg)
{
    Q_UNUSED(expression);
    Q_UNUSED(scanner);
    printf("PARSER ERROR: %s\n", msg);
}

%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%define api.pure
%lex-param { yyscan_t scanner }
%parse-param { KUserFeedback::SurveyTargetExpression **expression }
%parse-param { yyscan_t scanner }

%union {
    int intVal;
    double doubleVal;
    bool boolVal;
    char *str;
    KUserFeedback::SurveyTargetExpression *expression;
}

%left T_OP_OR
%left T_OP_AND

%token T_OP_EQ
%token T_OP_NEQ
%token T_OP_GT
%token T_OP_GE
%token T_OP_LT
%token T_OP_LE

%token T_DOT
%token T_LPAREN
%token T_RPAREN
%token T_LBRACKET
%token T_RBRACKET

%token <doubleVal> T_DOUBLE
%token <intVal> T_INTEGER
%token <boolVal> T_BOOLEAN
%token <str> T_STRING
%token <str> T_IDENTIFIER

%type <expression> Expr
%type <expression> Term
%type <expression> Value

%destructor { free($$); } <str>
%destructor { delete $$; } <expression>


%%

Input: Expr { *expression = $1; }
    ;

Expr: Term { printf("TERM "); $$ = $1; }
    | T_LPAREN Expr[E] T_RPAREN { $$ = $E; }
    | Expr[L] T_OP_AND Expr[R] %prec T_OP_AND { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpLogicAnd, $L, $R); }
    | Expr[L] T_OP_OR Expr[R]  %prec T_OP_OR  { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpLogicOr, $L, $R); }
;

Term: Value[L] T_OP_EQ Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpEqual, $L, $R); }
    | Value[L] T_OP_NEQ Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpNotEqual, $L, $R); }
    | Value[L] T_OP_GT Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpGreater, $L, $R); }
    | Value[L] T_OP_GE Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpGreaterEqual, $L, $R); }
    | Value[L] T_OP_LT Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpLess, $L, $R); }
    | Value[L] T_OP_LE Value[R] { $$ = new SurveyTargetExpression(SurveyTargetExpression::OpLessEqual, $L, $R); }
;

Value: T_IDENTIFIER[S] T_DOT T_IDENTIFIER[E] {
        $$ = new SurveyTargetExpression(QString::fromUtf8($S), QVariant(), QString::fromUtf8($E));
        free($S);
        free($E);
    }
    | T_IDENTIFIER[S] T_LBRACKET T_INTEGER[I] T_RBRACKET T_DOT T_IDENTIFIER[E] {
        $$ = new SurveyTargetExpression(QString::fromUtf8($S), $I, QString::fromUtf8($E));
        free($S);
        free($E);
    }
    | T_IDENTIFIER[S] T_LBRACKET T_STRING[K] T_RBRACKET T_DOT T_IDENTIFIER[E] {
        $$ = new SurveyTargetExpression(QString::fromUtf8($S), QString::fromUtf8($K), QString::fromUtf8($E));
        free($S);
        free($K);
        free($E);
    }
    | T_DOUBLE { $$ = new SurveyTargetExpression($1); }
    | T_INTEGER { $$ = new SurveyTargetExpression($1); }
    | T_BOOLEAN { $$ = new SurveyTargetExpression($1); }
    | T_STRING { $$ = new SurveyTargetExpression(QString::fromUtf8($1)); free($1); }
;

%%
