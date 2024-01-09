/*
  qmlcontextpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qmlcontextpropertyadaptor.h"

#include <core/propertydata.h>

#include <QQmlContext>
#include <private/qqmlcontext_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qqmlcontextdata_p.h>
#include <private/qv4identifierhashdata_p.h>
#else
#include <private/qv4identifier_p.h>
#endif

#include <QDebug>

using namespace GammaRay;

QmlContextPropertyAdaptor::QmlContextPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QmlContextPropertyAdaptor::~QmlContextPropertyAdaptor() = default;

int QmlContextPropertyAdaptor::count() const
{
    return ( int )m_contextPropertyNames.size();
}

PropertyData QmlContextPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;
    if (!object().isValid())
        return pd;

    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_contextPropertyNames.size());

    auto context = qobject_cast<QQmlContext *>(object().qtObject());
    if (!context)
        return pd;

    pd.setName(m_contextPropertyNames.at(index));
    pd.setValue(context->contextProperty(m_contextPropertyNames.at(index)));
    pd.setClassName(tr("QML Context Property"));
    pd.setAccessFlags(PropertyData::Writable);
    return pd;
}

void QmlContextPropertyAdaptor::writeProperty(int index, const QVariant &value)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_contextPropertyNames.size());

    const auto &name = m_contextPropertyNames.at(index);
    auto context = qobject_cast<QQmlContext *>(object().qtObject());
    if (name.isEmpty() || !context)
        return;
    context->setContextProperty(name, value);
}

void QmlContextPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto context = qobject_cast<QQmlContext *>(oi.qtObject());
    Q_ASSERT(context);

    auto contextData = QQmlContextData::get(context);
    Q_ASSERT(contextData);

    const auto &propNames = contextData->propertyNames();
    m_contextPropertyNames.clear();
    m_contextPropertyNames.reserve(propNames.count());

    QV4::IdentifierHashEntry *e = propNames.d->entries;
    QV4::IdentifierHashEntry *end = e + propNames.d->alloc;
    while (e < end) {
        if (e->identifier.isValid())
            m_contextPropertyNames.push_back(e->identifier.toQString());
        ++e;
    }
#else
    auto context = qobject_cast<QQmlContext *>(oi.qtObject());
    Q_ASSERT(context);
    auto contextData = QQmlContextData::get(context);
    Q_ASSERT(contextData);
    auto priv = contextData->asQQmlContextPrivate();
    Q_ASSERT(priv);

    const int numProps = priv->numPropertyValues();

#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    const auto propNames = contextData->propertyNames();
#endif

    for (int i = 0; i < numProps; ++i) {
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
        const auto prop = propNames.findId(i);
#else
        const auto prop = contextData->propertyName(i);
#endif
        if (!prop.isEmpty()) {
            m_contextPropertyNames.push_back(prop);
        }
    }
#endif
}

QmlContextPropertyAdaptorFactory *QmlContextPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QmlContextPropertyAdaptorFactory::create(const ObjectInstance &oi,
                                                          QObject *parent) const
{
    if (oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return nullptr;

    if (qobject_cast<QQmlContext *>(oi.qtObject()))
        return new QmlContextPropertyAdaptor(parent);

    return nullptr;
}

QmlContextPropertyAdaptorFactory *QmlContextPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QmlContextPropertyAdaptorFactory;
    return s_instance;
}
