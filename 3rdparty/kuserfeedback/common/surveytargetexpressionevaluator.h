/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_SURVEYTARGETEXPRESSIONEVALUATOR_H
#define KUSERFEEDBACK_SURVEYTARGETEXPRESSIONEVALUATOR_H

#include <QHash>

QT_BEGIN_NAMESPACE
class QString;
class QVariant;
QT_END_NAMESPACE

namespace KUserFeedback {

class SurveyTargetExpression;

class SurveyTargetExpressionDataProvider
{
public:
    SurveyTargetExpressionDataProvider();
    virtual ~SurveyTargetExpressionDataProvider();
    virtual QVariant sourceData(const QString &sourceName) const = 0;

private:
    Q_DISABLE_COPY(SurveyTargetExpressionDataProvider)
};

class SurveyTargetExpressionEvaluator
{
public:
    SurveyTargetExpressionEvaluator();
    ~SurveyTargetExpressionEvaluator();

    void setDataProvider(const SurveyTargetExpressionDataProvider *provider);

    bool evaluate(SurveyTargetExpression *expression);

private:
    QVariant value(SurveyTargetExpression *expression);
    QVariant value(const QString &source);

    const SurveyTargetExpressionDataProvider *m_provider;
    QHash<QString, QVariant> m_dataCache;
};

}

#endif // KUSERFEEDBACK_SURVEYTARGETEXPRESSIONEVALUATOR_H
