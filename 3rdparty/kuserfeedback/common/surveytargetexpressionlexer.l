%{
/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "surveytargetexpression.h"
#include "surveytargetexpressionparser_p.h"

extern char* unquoteString(const char *str);

%}
%option warn nodefault

%option reentrant
%option noyywrap
%option nounput
%option never-interactive
%option bison-bridge


SPACE       [ \t\r\n]+

DOUBLE      [-+]?([0-9]+[.][0-9]*|[0-9]*[.][0-9]+)([eE][-+]?[0-9]+)?
INTEGER     [-+]?[0-9]+
STRING_ELEMENT  [^\\"]|"\\\""|"\\\\"|"\\n"|"\\t"
STRING      ["]{STRING_ELEMENT}*["]

INITIAL     [[:alpha:]]|"_"
SUBSEQUENT  {INITIAL}|[0-9]
IDENTIFIER  {INITIAL}{SUBSEQUENT}*

%%

{SPACE} {}

"("  { return T_LPAREN; }
")"  { return T_RPAREN; }
"\." { return T_DOT; }
"["  { return T_LBRACKET; }
"]"  { return T_RBRACKET; }

{DOUBLE} {
    yylval->doubleVal = strtod(yytext, nullptr);
    return T_DOUBLE;
}
{INTEGER} {
    yylval->intVal = atoi(yytext);
    return T_INTEGER;
}
{STRING} {
    yylval->str = unquoteString(yytext);
    return T_STRING;
}
"true"  { yylval->boolVal = true; return T_BOOLEAN; }
"false" { yylval->boolVal = false; return T_BOOLEAN; }

{IDENTIFIER} {
    yylval->str = strdup(yytext);
    return T_IDENTIFIER;
}

"&&" { return T_OP_AND; }
"||" { return T_OP_OR; }
"==" { return T_OP_EQ; }
"!=" { return T_OP_NEQ; }
">"  { return T_OP_GT; }
">=" { return T_OP_GE; }
"<"  { return T_OP_LT; }
"<=" { return T_OP_LE; }

. {
    printf("unexpected character: %s\n", yytext);
}

%%

void yyerror(char const* s)
{
    printf("ERROR: %s\n", s);
}
