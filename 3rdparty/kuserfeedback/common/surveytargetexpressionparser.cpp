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
