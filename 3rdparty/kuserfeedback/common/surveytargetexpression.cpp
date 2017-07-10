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

using namespace KUserFeedback;

SurveyTargetExpression::SurveyTargetExpression(const QVariant &value)
    : m_type(Value)
    , m_value(value)
{
}

SurveyTargetExpression::SurveyTargetExpression(const QString& source, const QVariant &index, const QString& elem)
    : m_value(index)
    , m_source(source)
    , m_sourceElement(elem)
{
    if (index.type() == QVariant::Int)
        m_type = ListElement;
    else if (index.type() == QVariant::String)
        m_type = MapElement;
    else
        m_type = ScalarElement;
}

SurveyTargetExpression::SurveyTargetExpression(Type type, SurveyTargetExpression* left, SurveyTargetExpression* right)
    : m_type(type)
    , m_left(left)
    , m_right(right)
{
}

SurveyTargetExpression::~SurveyTargetExpression()
{
}

SurveyTargetExpression::Type SurveyTargetExpression::type() const
{
    return m_type;
}

QVariant SurveyTargetExpression::value() const
{
    return m_value;
}

QString SurveyTargetExpression::source() const
{
    return m_source;
}

QString SurveyTargetExpression::sourceElement() const
{
    return m_sourceElement;
}

SurveyTargetExpression* SurveyTargetExpression::left() const
{
    return m_left.get();
}

SurveyTargetExpression* SurveyTargetExpression::right() const
{
    return m_right.get();
}

QDebug operator<<(QDebug debug, SurveyTargetExpression* expr)
{
    if (!expr) {
        debug << "(null)";
        return debug;
    }

    switch (expr->type()) {
        case SurveyTargetExpression::Value:
            debug << expr->value().toString(); //toString() is needed for Qt4 support
            break;
        case SurveyTargetExpression::ScalarElement:
            debug.nospace() << expr->source() << "." << expr->sourceElement();
            break;
        case SurveyTargetExpression::ListElement:
            debug.nospace() << expr->source() << "[" << expr->value().toInt() << "]." << expr->sourceElement();
            break;
        case SurveyTargetExpression::MapElement:
            debug.nospace() << expr->source() << "[" << expr->value().toString() << "]." << expr->sourceElement();
            break;
        case SurveyTargetExpression::OpLogicAnd:
            debug.nospace() << "(" << expr->left() << " && " << expr->right() << ")";
            break;
        case SurveyTargetExpression::OpLogicOr:
            debug.nospace() << "(" << expr->left() << " || " << expr->right() << ")";
            break;
        case SurveyTargetExpression::OpEqual:
            debug.nospace() << "[" << expr->left() << " == " << expr->right() << "]";
            break;
        case SurveyTargetExpression::OpNotEqual:
            debug.nospace() << "[" << expr->left() << " != " << expr->right() << "]";
            break;
        case SurveyTargetExpression::OpGreater:
            debug.nospace() << "[" << expr->left() << " > " << expr->right() << "]";
            break;
        case SurveyTargetExpression::OpGreaterEqual:
            debug.nospace() << "[" << expr->left() << " >= " << expr->right() << "]";
            break;
        case SurveyTargetExpression::OpLess:
            debug.nospace() << "[" << expr->left() << " < " << expr->right() << "]";
            break;
        case SurveyTargetExpression::OpLessEqual:
            debug.nospace() << "[" << expr->left() << " <= " << expr->right() << "]";
            break;
    }

    return debug;
}
