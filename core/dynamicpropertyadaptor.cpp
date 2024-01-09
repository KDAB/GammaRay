/*
  dynamicpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "dynamicpropertyadaptor.h"
#include "propertydata.h"
#include "objectinstance.h"

#include <QDebug>
#include <QEvent>

using namespace GammaRay;

DynamicPropertyAdaptor::DynamicPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

DynamicPropertyAdaptor::~DynamicPropertyAdaptor() = default;

void DynamicPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    auto obj = oi.qtObject();
    if (obj) {
        m_propNames = obj->dynamicPropertyNames();
        obj->installEventFilter(this);
        connect(obj, &QObject::destroyed, this, &PropertyAdaptor::objectInvalidated);
    }
}

int DynamicPropertyAdaptor::count() const
{
    if (!object().isValid())
        return 0;
    Q_ASSERT(m_propNames.size() == object().qtObject()->dynamicPropertyNames().size());
    return m_propNames.size();
}

PropertyData DynamicPropertyAdaptor::propertyData(int index) const
{
    PropertyData data;
    if (!object().isValid())
        return data;

    Q_ASSERT(m_propNames.size() == object().qtObject()->dynamicPropertyNames().size());

    data.setName(m_propNames.at(index));
    data.setValue(object().qtObject()->property(m_propNames.at(index)));
    data.setClassName(tr("<dynamic>"));
    data.setAccessFlags(PropertyData::Writable | PropertyData::Deletable);
    return data;
}

void DynamicPropertyAdaptor::writeProperty(int index, const QVariant &value)
{
    if (!object().isValid())
        return;
    Q_ASSERT(index < m_propNames.size());

    const auto &propName = m_propNames.at(index);
    object().qtObject()->setProperty(propName, value);
}

bool DynamicPropertyAdaptor::canAddProperty() const
{
    return object().qtObject();
}

void DynamicPropertyAdaptor::addProperty(const PropertyData &data)
{
    if (!object().isValid())
        return;
    Q_ASSERT(!m_propNames.contains(data.name().toUtf8()));

    object().qtObject()->setProperty(data.name().toUtf8(), data.value());
}

bool DynamicPropertyAdaptor::eventFilter(QObject *receiver, QEvent *event)
{
    auto obj = object().qtObject();
    if (receiver == obj && event->type() == QEvent::DynamicPropertyChange) {
        const auto changeEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
        const auto oldIdx = m_propNames.indexOf(changeEvent->propertyName());
        const auto newIdx = obj->dynamicPropertyNames().indexOf(changeEvent->propertyName());
        if (oldIdx >= 0 && newIdx >= 0) {
            Q_ASSERT(oldIdx == newIdx);
            emit propertyChanged(oldIdx, oldIdx);
        } else if (newIdx >= 0) {
            m_propNames = obj->dynamicPropertyNames();
            emit propertyAdded(newIdx, newIdx);
        } else {
            Q_ASSERT(oldIdx >= 0);
            m_propNames = obj->dynamicPropertyNames();
            emit propertyRemoved(oldIdx, oldIdx);
        }
    }

    return PropertyAdaptor::eventFilter(receiver, event);
}
