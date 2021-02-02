/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
