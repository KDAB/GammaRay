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
