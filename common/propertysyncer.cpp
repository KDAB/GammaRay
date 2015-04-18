/*
  propertysyncer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <QDebug>
#include <QMetaProperty>

#include <algorithm>

using namespace GammaRay;

static int qobjectPropertyOffset()
{
    return QObject::staticMetaObject.propertyCount();
}

PropertySyncer::PropertySyncer(QObject* parent) :
    QObject(parent),
    m_address(Protocol::InvalidObjectAddress),
    m_initialSync(false)
{
}

PropertySyncer::~PropertySyncer()
{
}

void PropertySyncer::setRequestInitialSync(bool initialSync)
{
    m_initialSync = initialSync;
}

void PropertySyncer::addObject(Protocol::ObjectAddress addr, QObject* obj)
{
    Q_ASSERT(addr != Protocol::InvalidObjectAddress);
    Q_ASSERT(obj);

    bool hasProperties = false;
    for (int i = qobjectPropertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        const auto prop = obj->metaObject()->property(i);
        if (!prop.hasNotifySignal())
            continue;
        connect(obj, QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
          prop.notifySignal().signature()
#else
          prop.notifySignal().methodSignature()
#endif
          , this, SLOT(propertyChanged()));
        hasProperties = true;
    }

    if (!hasProperties) {
        qDebug() << "no properties" << obj;
        return;
    }
    qDebug() << "found properties in" << obj << obj->metaObject()->propertyOffset() << obj->metaObject()->propertyCount();

    connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));

    ObjectInfo info;
    info.addr = addr;
    info.obj = obj;
    info.recursionLock = false;
    info.enabled = false;
    m_objects.push_back(info);
}

void PropertySyncer::setObjectEnabled(Protocol::ObjectAddress addr, bool enabled)
{
    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [addr](const ObjectInfo &info) {
        return info.addr == addr;
    });
    if (it == m_objects.end() || (*it).enabled == enabled)
        return;

    (*it).enabled = enabled;
    if (enabled && m_initialSync) {
        Message msg(m_address, Protocol::PropertySyncRequest);
        msg.payload() << addr;
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

void PropertySyncer::handleMessage(const GammaRay::Message& msg)
{
    Q_ASSERT(msg.address() == m_address);
    switch (msg.type()) {
        case Protocol::PropertySyncRequest:
        {
            Protocol::ObjectAddress addr;
            msg.payload() >> addr;
            Q_ASSERT(addr != Protocol::InvalidObjectAddress);

            const auto it = std::find_if(m_objects.constBegin(), m_objects.constEnd(), [addr](const ObjectInfo &info) {
                return info.addr == addr;
            });
            if (it == m_objects.constEnd())
                break;

            qDebug() << "sync request for" << (*it).obj << (*it).obj->metaObject()->propertyOffset() << (*it).obj->metaObject()->propertyCount();
            QVector<QPair<QString, QVariant> > values;
            for (int i = qobjectPropertyOffset(); i < (*it).obj->metaObject()->propertyCount(); ++i) {
                const auto prop = (*it).obj->metaObject()->property(i);
                values.push_back(qMakePair(QString(prop.name()), prop.read((*it).obj)));
            }
            Q_ASSERT(!values.isEmpty());

            Message msg(m_address, Protocol::PropertyValuesChanged);
            msg.payload() << addr << (quint32)values.size();
            foreach (const auto &value, values)
                msg.payload() << value.first << value.second;
            emit message(msg);
            break;
        }
        case Protocol::PropertyValuesChanged:
        {
            Protocol::ObjectAddress addr;
            quint32 changeSize;
            msg.payload() >> addr >> changeSize;
            Q_ASSERT(addr != Protocol::InvalidObjectAddress);
            Q_ASSERT(changeSize > 0);

            const auto it = std::find_if(m_objects.begin(), m_objects.end(), [addr](const ObjectInfo &info) {
                return info.addr == addr;
            });
            if (it == m_objects.end())
                break;

            for (quint32 i = 0; i < changeSize; ++i) {
                QString propName;
                QVariant propValue;
                msg.payload() >> propName >> propValue;
                qDebug() << propName << propValue;
                (*it).recursionLock = true;
                (*it).obj->setProperty(propName.toUtf8(), propValue);
                (*it).recursionLock = false;
            }
            break;
        }
        default:
            Q_ASSERT(!"We should not get here!");
    }
}

void PropertySyncer::propertyChanged()
{
    const auto *obj = sender();
    Q_ASSERT(obj);
    const auto it = std::find_if(m_objects.constBegin(), m_objects.constEnd(), [obj](const ObjectInfo &info) {
        return info.obj == obj;
    });
    Q_ASSERT(it != m_objects.constEnd());

    if ((*it).recursionLock || !(*it).enabled)
        return;

    const auto sigIndex = senderSignalIndex();
    QVector<QPair<QString, QVariant> > changes;
    for (int i = qobjectPropertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        const auto prop = obj->metaObject()->property(i);
        if (prop.notifySignalIndex() != sigIndex)
            continue;
        qDebug() << prop.name() << "changed";
        changes.push_back(qMakePair(QString(prop.name()), prop.read(obj)));
    }
    Q_ASSERT(!changes.isEmpty());

    Message msg(m_address, Protocol::PropertyValuesChanged);
    msg.payload() << (*it).addr << (quint32)changes.size();
    foreach (const auto &change, changes)
        msg.payload() << change.first << change.second;
    emit message(msg);
}

void PropertySyncer::objectDestroyed(QObject* obj)
{
    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const ObjectInfo &info) {
        return info.obj == obj;
    });
    Q_ASSERT(it != m_objects.end());
    m_objects.erase(it);
}
