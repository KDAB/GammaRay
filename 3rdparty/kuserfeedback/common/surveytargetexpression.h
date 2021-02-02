/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
