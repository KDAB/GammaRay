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
