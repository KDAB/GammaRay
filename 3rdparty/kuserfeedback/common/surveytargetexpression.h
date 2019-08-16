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

#ifndef KUSERFEEDBACK_SURVEYTARGETEXPRESSION_H
#define KUSERFEEDBACK_SURVEYTARGETEXPRESSION_H

#include <QDebug>
#include <QString>
#include <QVariant>

#include <memory>

namespace KUserFeedback {

class SurveyTargetExpression
{
public:
    enum Type {
        Value,
        ScalarElement,
        ListElement,
        MapElement,

        OpLogicAnd,
        OpLogicOr,
        OpEqual,
        OpNotEqual,
        OpLess,
        OpLessEqual,
        OpGreater,
        OpGreaterEqual
    };

    explicit SurveyTargetExpression(const QVariant &value);
    explicit SurveyTargetExpression(const QString &source, const QVariant &index, const QString &elem);
    explicit SurveyTargetExpression(Type type, SurveyTargetExpression *left, SurveyTargetExpression *right);
    ~SurveyTargetExpression();

    Type type() const;
    QVariant value() const;

    QString source() const;
    QString sourceElement() const;

    SurveyTargetExpression* left() const;
    SurveyTargetExpression* right() const;

private:
    Type m_type;
    QVariant m_value;
    QString m_source;
    QString m_sourceElement;
    std::unique_ptr<SurveyTargetExpression> m_left;
    std::unique_ptr<SurveyTargetExpression> m_right;
};

}

QDebug operator<<(QDebug debug, KUserFeedback::SurveyTargetExpression *expr);

#endif
