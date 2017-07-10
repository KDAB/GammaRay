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

#ifndef KUSERFEEDBACK_SURVEYTARGETEXPRESSIONPARSER_H
#define KUSERFEEDBACK_SURVEYTARGETEXPRESSIONPARSER_H

#include <QtGlobal>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace KUserFeedback {

class SurveyTargetExpression;

class SurveyTargetExpressionParser
{
public:
    SurveyTargetExpressionParser();
    ~SurveyTargetExpressionParser();

    bool parse(const QString &s);

    SurveyTargetExpression *expression() const;

private:
    Q_DISABLE_COPY(SurveyTargetExpressionParser)
    SurveyTargetExpression *m_expression;
};

}

#endif // KUSERFEEDBACK_SURVEYTARGETEXPRESSIONPARSER_H
