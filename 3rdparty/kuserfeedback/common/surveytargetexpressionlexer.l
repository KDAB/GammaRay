%{
/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    yylval->doubleVal = atof(yytext);
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
