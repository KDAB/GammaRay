/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <config-userfeedback.h>

#include "surveytargetexpressionparser.h"
#include "surveytargetexpression.h"

#ifdef HAVE_SURVEY_TARGET_EXPRESSIONS
#include "surveytargetexpressionparser_p.h"
#include "surveytargetexpressionscanner.h"
#endif

#include <QString>

#include <cstring>

char* unquoteString(const char *str)
{
    const auto size = strlen(str) - 2;
    if (size <= 0)
        return nullptr;
    auto out = (char*)malloc(size + 1);
    memset(out, 0, size + 1);
    auto outIt = out;
    for (auto it = str + 1; it < str + size + 1; ++it, ++outIt) {
        if (*it == '\\') {
            ++it;
            switch (*it) {
                case '\\':
                case '"':
                    *outIt = *it; break;
                case 'n':
                    *outIt = '\n'; break;
                case 't':
                    *outIt = '\t'; break;
                default:
                    *outIt++ = '\\';
                    *outIt = *it;
            }
        } else {
            *outIt = *it;
        }
    }
    return out;
}

using namespace KUserFeedback;

SurveyTargetExpressionParser::SurveyTargetExpressionParser()
    : m_expression(nullptr)
{
}

SurveyTargetExpressionParser::~SurveyTargetExpressionParser()
{
    delete m_expression;
}

bool SurveyTargetExpressionParser::parse(const QString &s)
{
    delete m_expression;
    m_expression = nullptr;

#ifdef HAVE_SURVEY_TARGET_EXPRESSIONS
    yyscan_t scanner;
    YY_BUFFER_STATE state;
    if (yylex_init(&scanner))
        return false;

    QByteArray b = s.toUtf8();
    state = yy_scan_string(b.constData(), scanner);
    if (yyparse(&m_expression, scanner)) {
        delete m_expression;
        m_expression = nullptr;
    }

    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);
#endif
    return m_expression;
}

SurveyTargetExpression* SurveyTargetExpressionParser::expression() const
{
    return m_expression;
}
