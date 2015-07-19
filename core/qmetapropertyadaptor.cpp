/*
  qmetapropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qmetapropertyadaptor.h"
#include "objectinstance.h"
#include "propertydata.h"
#include "util.h"

#include <QMetaProperty>
#include <QStringList>

using namespace GammaRay;

QMetaPropertyAdaptor::QMetaPropertyAdaptor(QObject* parent): PropertyAdaptor(parent)
{
}

QMetaPropertyAdaptor::~QMetaPropertyAdaptor()
{
}

void QMetaPropertyAdaptor::doSetObject(const ObjectInstance& oi)
{
    auto mo = oi.metaObject();
    if (!mo || oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return;

    connect(oi.qtObject(), SIGNAL(destroyed(QObject*)), this, SIGNAL(objectInvalidated()));

    for (int i = 0; i < mo->propertyCount(); ++i) {
        const QMetaProperty prop = mo->property(i);
        if (prop.hasNotifySignal()) {
            connect(oi.qtObject(), QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
                prop.notifySignal().signature()
#else
                prop.notifySignal().methodSignature()
#endif
                , this, SLOT(propertyUpdated()));
            m_notifyToPropertyMap.insert(prop.notifySignalIndex(), i);
        }
    }
}

int QMetaPropertyAdaptor::count() const
{
    if (!object().isValid())
        return 0;

    auto mo = object().metaObject();
    if (!mo)
        return 0;
    return mo->propertyCount();
}

static QString translateBool(bool value)
{
    static const QString yesStr = QMetaPropertyAdaptor::tr("yes");
    static const QString noStr = QMetaPropertyAdaptor::tr("no");
    return value ? yesStr : noStr;
}

QString QMetaPropertyAdaptor::detailString(const QMetaProperty& prop) const
{
    QObject *obj = object().qtObject();
    QStringList s;
    s << tr("Constant: %1").arg(translateBool(prop.isConstant()));
    s << tr("Designable: %1").arg(translateBool(prop.isDesignable(obj)));
    s << tr("Final: %1").arg(translateBool(prop.isFinal()));
    if (prop.hasNotifySignal()) {
        s << tr("Notification: %1").arg(Util::prettyMethodSignature(prop.notifySignal()));
    } else {
        s << tr("Notification: no");
    }
    s << tr("Resetable: %1").arg(translateBool(prop.isResettable()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    s << tr("Revision: %1").arg(prop.revision());
#endif
    s << tr("Scriptable: %1").arg(translateBool(prop.isScriptable(obj)));
    s << tr("Stored: %1").arg(translateBool(prop.isStored(obj)));
    s << tr("User: %1").arg(translateBool(prop.isUser(obj)));
    s << tr("Writable: %1").arg(translateBool(prop.isWritable()));
    return s.join("\n");
}

PropertyData QMetaPropertyAdaptor::propertyData(int index) const
{
    PropertyData data;
    if (!object().isValid())
        return data;

    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(index);

    data.setName(prop.name());
    data.setTypeName(prop.typeName());

    auto pmo = mo;
    while (pmo->propertyOffset() > index)
        pmo = pmo->superClass();
    data.setClassName(pmo->className());

    switch (object().type()) {
        case ObjectInstance::QtObject:
            if (object().qtObject())
                data.setValue(prop.read(object().qtObject()));
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case ObjectInstance::QtGadget:
            if (object().object())
                data.setValue(prop.readOnGadget(object().object()));
            break;
#endif
    }

    data.setDetails(detailString(prop));

    PropertyData::Flags flags = PropertyData::Readable;
    if (prop.isWritable())
        flags |= PropertyData::Writable;
    if (prop.isResettable())
        flags |= PropertyData::Resettable;
    data.setFlags(flags);

    return data;
}

void QMetaPropertyAdaptor::writeProperty(int index, const QVariant& value)
{
    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(index);
    switch (object().type()) {
        case ObjectInstance::QtObject:
            if (object().qtObject()) {
                prop.write(object().qtObject(), value);
                if (!prop.hasNotifySignal())
                    emit propertyChanged(index, index);
            }
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case ObjectInstance::QtGadget:
            if (object().object()) {
                prop.writeOnGadget(object().object(), value);
                emit propertyChanged(index, index);
            }
            break;
#endif
    }
}

void QMetaPropertyAdaptor::resetProperty(int index)
{
    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(index);
    switch (object().type()) {
        case ObjectInstance::QtObject:
            if (object().qtObject()) {
                prop.reset(object().qtObject());
                if (!prop.hasNotifySignal())
                    emit propertyChanged(index, index);
            }
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case ObjectInstance::QtGadget:
            if (object().object()) {
                prop.resetOnGadget(object().object());
                emit propertyChanged(index, index);
            }
            break;
#endif
    }
}

void QMetaPropertyAdaptor::propertyUpdated()
{
    Q_ASSERT(senderSignalIndex() >= 0);
    const int propertyIndex = m_notifyToPropertyMap.value(senderSignalIndex());
    emit propertyChanged(propertyIndex, propertyIndex);
}
