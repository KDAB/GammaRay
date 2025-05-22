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

#include <private/qqmlcontext_p.h>
#include <private/qqmlcontextdata_p.h>
#include <private/qv4identifierhashdata_p.h>

#include <QDebug>
#include <QQmlContext>

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
    auto context = qobject_cast<QQmlContext *>(oi.qtObject());
    Q_ASSERT(context);
    auto contextData = QQmlContextData::get(context);
    Q_ASSERT(contextData);
    auto priv = contextData->asQQmlContextPrivate();
    Q_ASSERT(priv);

    const int numProps = priv->numPropertyValues();

    for (int i = 0; i < numProps; ++i) {
        const auto prop = contextData->propertyName(i);
        if (!prop.isEmpty()) {
            m_contextPropertyNames.push_back(prop);
        }
    }
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
