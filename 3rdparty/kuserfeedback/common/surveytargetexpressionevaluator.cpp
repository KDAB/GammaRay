/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "surveytargetexpressionevaluator.h"
#include "surveytargetexpression.h"

using namespace KUserFeedback;

SurveyTargetExpressionDataProvider::SurveyTargetExpressionDataProvider()
{
}

SurveyTargetExpressionDataProvider::~SurveyTargetExpressionDataProvider()
{
}

SurveyTargetExpressionEvaluator::SurveyTargetExpressionEvaluator()
    : m_provider(nullptr)
{
}

SurveyTargetExpressionEvaluator::~SurveyTargetExpressionEvaluator()
{
}

void SurveyTargetExpressionEvaluator::setDataProvider(const SurveyTargetExpressionDataProvider* provider)
{
    m_provider = provider;
}

static bool variantLess(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QVariant::compare(lhs, rhs) == QPartialOrdering::Less;
#else
    return lhs < rhs;
#endif
}

static bool variantLessOrEqual(const QVariant &lhs, const QVariant &rhs)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto order = QVariant::compare(lhs, rhs);
    return order == QPartialOrdering::Less || order == QPartialOrdering::Equivalent;
#else
    return lhs <= rhs;
#endif
}

bool SurveyTargetExpressionEvaluator::evaluate(SurveyTargetExpression* expression)
{
    // logical operations
    switch (expression->type()) {
        case SurveyTargetExpression::OpLogicAnd:
            return evaluate(expression->left()) && evaluate(expression->right());
        case SurveyTargetExpression::OpLogicOr:
            return evaluate(expression->left()) || evaluate(expression->right());
        default:
            break;
    }

    // comparisson operations:
    const auto lhs = value(expression->left());
    const auto rhs = value(expression->right());
    if (lhs.type() == QVariant::Invalid || rhs.type() == QVariant::Invalid) // invalid element access can never succeed
        return false;
    if ((lhs.type() == QVariant::String && rhs.type() != QVariant::String)
        || (lhs.type() != QVariant::String && rhs.type() == QVariant::String))
        return false; // strings can only be compared to strings

    switch (expression->type()) {
        case SurveyTargetExpression::OpEqual:
            return lhs == rhs;
        case SurveyTargetExpression::OpNotEqual:
            return lhs != rhs;
        case SurveyTargetExpression::OpGreater:
            return variantLess(rhs, lhs);
        case SurveyTargetExpression::OpGreaterEqual:
            return variantLessOrEqual(rhs, lhs);
        case SurveyTargetExpression::OpLess:
            return variantLess(lhs, rhs);
        case SurveyTargetExpression::OpLessEqual:
            return variantLessOrEqual(lhs, rhs);
        default:
            break;
    }

    return false;
}

QVariant SurveyTargetExpressionEvaluator::value(SurveyTargetExpression* expression)
{
    switch (expression->type()) {
        case SurveyTargetExpression::Value:
            return expression->value();
        case SurveyTargetExpression::ScalarElement:
        {
            const auto v = value(expression->source());
            if (v.canConvert<QVariantList>() && expression->sourceElement() == QLatin1String("size"))
                return v.value<QVariantList>().size();
            const auto m = v.toMap();
            const auto it = m.find(expression->sourceElement());
            if (it != m.end() && !it.value().canConvert<QVariantMap>())
                return it.value();
            if (expression->sourceElement() == QLatin1String("size"))
                return m.size();
            return QVariant();
        }
        case SurveyTargetExpression::ListElement:
        {
            const auto v = value(expression->source()).value<QVariantList>().value(expression->value().toInt());
            return v.toMap().value(expression->sourceElement());
        }
        case SurveyTargetExpression::MapElement:
        {
            const auto v = value(expression->source()).toMap().value(expression->value().toString());
            qDebug() << v << value(expression->source()).toMap() << expression->value().toString();
            return v.toMap().value(expression->sourceElement());
        }
        default:
            break;
    }

    Q_ASSERT(false);
    return QVariant();
}

QVariant SurveyTargetExpressionEvaluator::value(const QString& source)
{
    Q_ASSERT(m_provider);
    const auto it = m_dataCache.constFind(source);
    if (it != m_dataCache.constEnd())
        return it.value();

    const auto v = m_provider->sourceData(source);
    m_dataCache.insert(source, v);
    return v;
}
