/*
  qmlcontextpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qmlcontextpropertyadaptor.h"

#include <core/propertydata.h>

#include <QQmlContext>
#include <private/qqmlcontext_p.h>
#include <private/qv4identifier_p.h>

#include <QDebug>

using namespace GammaRay;

QmlContextPropertyAdaptor::QmlContextPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QmlContextPropertyAdaptor::~QmlContextPropertyAdaptor() = default;

int QmlContextPropertyAdaptor::count() const
{
    return m_contextPropertyNames.size();
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

    const auto name = m_contextPropertyNames.at(index);
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

    const auto &propNames = contextData->propertyNames();
    m_contextPropertyNames.clear();
    m_contextPropertyNames.reserve(propNames.count());

    QV4::IdentifierHashEntry *e = propNames.d->entries;
    QV4::IdentifierHashEntry *end = e + propNames.d->alloc;
    while (e < end) {
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
        if (e->identifier)
            m_contextPropertyNames.push_back(e->identifier->string);
#else
        if (e->identifier.isValid())
            m_contextPropertyNames.push_back(e->identifier.toQString());
#endif
        ++e;
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
