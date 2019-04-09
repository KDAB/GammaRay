/*
  propertysyncer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertysyncer.h"
#include "message.h"

#include <compat/qasconst.h>

#include <QDebug>
#include <QMetaProperty>

#include <algorithm>

using namespace GammaRay;

static int qobjectPropertyOffset()
{
    return QObject::staticMetaObject.propertyCount();
}

PropertySyncer::PropertySyncer(QObject *parent)
    : QObject(parent)
    , m_address(Protocol::InvalidObjectAddress)
    , m_initialSync(false)
{
}

PropertySyncer::~PropertySyncer() = default;

void PropertySyncer::setRequestInitialSync(bool initialSync)
{
    m_initialSync = initialSync;
}

void PropertySyncer::addObject(Protocol::ObjectAddress addr, QObject *obj)
{
    Q_ASSERT(addr != Protocol::InvalidObjectAddress);
    Q_ASSERT(obj);
    if (qobjectPropertyOffset() == obj->metaObject()->propertyCount())
        return; // no properties we could sync

    for (int i = qobjectPropertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        const auto prop = obj->metaObject()->property(i);
        if (!prop.hasNotifySignal())
            continue;
        connect(obj, QByteArray("2") + prop.notifySignal().methodSignature(), this, SLOT(propertyChanged()));
    }

    connect(obj, &QObject::destroyed, this, &PropertySyncer::objectDestroyed);

    ObjectInfo info;
    info.addr = addr;
    info.obj = obj;
    info.recursionLock = false;
    info.enabled = false;
    m_objects.push_back(info);
}

void PropertySyncer::setObjectEnabled(Protocol::ObjectAddress addr, bool enabled)
{
    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [addr](
                                     const ObjectInfo &info) {
        return info.addr == addr;
    });
    if (it == m_objects.end() || (*it).enabled == enabled)
        return;

    (*it).enabled = enabled;
    if (enabled && m_initialSync) {
        Message msg(m_address, Protocol::PropertySyncRequest);
        msg << addr;
        emit message(msg);
    }
}

Protocol::ObjectAddress PropertySyncer::address() const
{
    return m_address;
}

void PropertySyncer::setAddress(Protocol::ObjectAddress addr)
{
    m_address = addr;
}

void PropertySyncer::handleMessage(const GammaRay::Message &msg)
{
    Q_ASSERT(msg.address() == m_address);
    switch (msg.type()) {
    case Protocol::PropertySyncRequest:
    {
        Protocol::ObjectAddress addr;
        msg >> addr;
        Q_ASSERT(addr != Protocol::InvalidObjectAddress);

        const auto it
            = std::find_if(m_objects.constBegin(), m_objects.constEnd(),
                           [addr](const ObjectInfo &info) {
                return info.addr == addr;
            });
        if (it == m_objects.constEnd())
            break;

        QVector<QPair<QByteArray, QVariant> > values;
        const auto propCount = (*it).obj->metaObject()->propertyCount();
        values.reserve(propCount);
        for (int i = qobjectPropertyOffset(); i < propCount; ++i) {
            const auto prop = (*it).obj->metaObject()->property(i);
            values.push_back(qMakePair(QByteArray(prop.name()), prop.read((*it).obj)));
        }
        Q_ASSERT(!values.isEmpty());

        Message msg(m_address, Protocol::PropertyValuesChanged);
        msg << addr << (quint32)values.size();
        for (const auto &value : qAsConst(values))
            msg << value.first << value.second;
        emit message(msg);
        break;
    }
    case Protocol::PropertyValuesChanged:
    {
        Protocol::ObjectAddress addr;
        quint32 changeSize;
        msg >> addr >> changeSize;
        Q_ASSERT(addr != Protocol::InvalidObjectAddress);
        Q_ASSERT(changeSize > 0);

        auto it = std::find_if(m_objects.begin(), m_objects.end(), [addr](const ObjectInfo &info) {
                return info.addr == addr;
            });
        if (it == m_objects.end())
            break;

        for (quint32 i = 0; i < changeSize; ++i) {
            QByteArray propName;
            QVariant propValue;
            msg >> propName >> propValue;
            (*it).recursionLock = true;
            (*it).obj->setProperty(propName, propValue);

            // it can be invalid if as a result of the above call new objects have been registered for example
            it = std::find_if(m_objects.begin(), m_objects.end(), [addr](const ObjectInfo &info) {
                    return info.addr == addr;
                });
            Q_ASSERT(it != m_objects.end());
            (*it).recursionLock = false;
        }
        break;
    }
    default:
        Q_ASSERT_X(false, "PropertySyncer::handleMessage",
                   "Unexpected Gammaray::Message type encountered");
    }
}

void PropertySyncer::propertyChanged()
{
    const auto *obj = sender();
    Q_ASSERT(obj);
    const auto it
        = std::find_if(m_objects.constBegin(), m_objects.constEnd(), [obj](const ObjectInfo &info) {
        return info.obj == obj;
    });
    Q_ASSERT(it != m_objects.constEnd());

    if ((*it).recursionLock || !(*it).enabled)
        return;

    const auto sigIndex = senderSignalIndex();
    QVector<QPair<QByteArray, QVariant> > changes;
    for (int i = qobjectPropertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        const auto prop = obj->metaObject()->property(i);
        if (prop.notifySignalIndex() != sigIndex)
            continue;
        changes.push_back(qMakePair(QByteArray(prop.name()), prop.read(obj)));
    }
    Q_ASSERT(!changes.isEmpty());

    Message msg(m_address, Protocol::PropertyValuesChanged);
    msg << (*it).addr << (quint32)changes.size();
    for (const auto &change : qAsConst(changes))
        msg << change.first << change.second;
    emit message(msg);
}

void PropertySyncer::objectDestroyed(QObject *obj)
{
    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const ObjectInfo &info) {
        return info.obj == obj;
    });
    Q_ASSERT(it != m_objects.end());
    m_objects.erase(it);
}
