/*
  qmetapropertyadaptor.cpp

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

#include "qmetapropertyadaptor.h"
#include "propertyfilter.h"
#include "objectinstance.h"
#include "propertydata.h"
#include "util.h"
#include "probeguard.h"

#include <QMetaProperty>
#include <QStringList>

using namespace GammaRay;

QMetaPropertyAdaptor::QMetaPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
    , m_notifyGuard(false)
{
}

QMetaPropertyAdaptor::~QMetaPropertyAdaptor() = default;

void QMetaPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    auto mo = oi.metaObject();
    if (!mo)
        return;

    if (oi.type() == ObjectInstance::QtObject && oi.qtObject())
        connect(oi.qtObject(), &QObject::destroyed, this, &PropertyAdaptor::objectInvalidated);

    for (int i = 0; i < mo->propertyCount(); ++i) {
        const QMetaProperty prop = mo->property(i);
        if (!PropertyFilters::matches(propertyMetaData(i))) {
            if (oi.type() == ObjectInstance::QtObject && oi.qtObject() && prop.hasNotifySignal()) {
                connect(oi.qtObject(), QByteArray("2") + prop.notifySignal().methodSignature(), this, SLOT(propertyUpdated()));
                m_notifyToRowMap.insert(prop.notifySignalIndex(), m_rowToPropertyIndex.size());
            }
            m_rowToPropertyIndex.push_back(i);
        }
    }
}

int QMetaPropertyAdaptor::count() const
{
    if (!object().isValid())
        return 0;

    return m_rowToPropertyIndex.count();
}

PropertyData QMetaPropertyAdaptor::propertyMetaData(int propertyIndex) const
{
    PropertyData data;
    if (!object().isValid())
        return data;

    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(propertyIndex);

    data.setName(prop.name());
    data.setTypeName(prop.typeName());

    auto pmo = mo;
    while (pmo->propertyOffset() > propertyIndex)
        pmo = pmo->superClass();
    data.setClassName(pmo->className());

    PropertyModel::PropertyFlags f(PropertyModel::None);
    if (prop.isConstant())
        f |= PropertyModel::Constant;
    if (prop.isDesignable(object().qtObject()))
        f |= PropertyModel::Designable;
    if (prop.isFinal())
        f |= PropertyModel::Final;
    if (prop.isResettable())
        f |= PropertyModel::Resetable;
    if (prop.isScriptable(object().qtObject()))
        f |= PropertyModel::Scriptable;
    if (prop.isStored(object().qtObject()))
        f |= PropertyModel::Stored;
    if (prop.isUser(object().qtObject()))
        f |= PropertyModel::User;
    if (prop.isWritable())
        f |= PropertyModel::Writable;
    data.setPropertyFlags(f);
    data.setRevision(prop.revision());
    if (prop.hasNotifySignal())
        data.setNotifySignal(Util::prettyMethodSignature(prop.notifySignal()));

    PropertyData::AccessFlags flags = PropertyData::Readable;
    if (prop.isWritable())
        flags |= PropertyData::Writable;
    if (prop.isResettable())
        flags |= PropertyData::Resettable;
    data.setAccessFlags(flags);

    return data;
}

PropertyData QMetaPropertyAdaptor::propertyData(int row) const
{
    int propertyIndex = m_rowToPropertyIndex[row];

    PropertyData data = propertyMetaData(propertyIndex);
    if (!object().isValid())
        return data;

    m_notifyGuard = true;
    const auto mo = object().metaObject();
    Q_ASSERT(mo);
    const auto prop = mo->property(propertyIndex);

    // we call out to the target here, so suspend the probe guard, otherwise we'll miss on-demand created object (e.g. in QQ2)
    {
        ProbeGuardSuspender g;
        switch (object().type()) {
        case ObjectInstance::QtObject:
            if (object().qtObject())
                data.setValue(prop.read(object().qtObject()));
            break;
        case ObjectInstance::QtGadgetPointer:
        case ObjectInstance::QtGadgetValue:
            if (object().object())
                data.setValue(prop.readOnGadget(object().object()));
            break;
        default:
            break;
        }
    }

    m_notifyGuard = false;
    return data;
}

void QMetaPropertyAdaptor::writeProperty(int row, const QVariant &value)
{
    int propertyIndex = m_rowToPropertyIndex[row];
    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(propertyIndex);
    switch (object().type()) {
    case ObjectInstance::QtObject:
        if (object().qtObject()) {
            prop.write(object().qtObject(), value);
            if (!prop.hasNotifySignal())
                emit propertyChanged(row, row);
        }
        break;
    case ObjectInstance::QtGadgetPointer:
    case ObjectInstance::QtGadgetValue:
        if (object().object()) {
            prop.writeOnGadget(object().object(), value);
            emit propertyChanged(row, row);
        }
        break;
    default:
        break;
    }
}

void QMetaPropertyAdaptor::resetProperty(int row)
{
    int propertyIndex = m_rowToPropertyIndex[row];
    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(propertyIndex);
    switch (object().type()) {
    case ObjectInstance::QtObject:
        if (object().qtObject()) {
            prop.reset(object().qtObject());
            if (!prop.hasNotifySignal())
                emit propertyChanged(row, row);
        }
        break;
    case ObjectInstance::QtGadgetValue:
    case ObjectInstance::QtGadgetPointer:
        if (object().object()) {
            prop.resetOnGadget(object().object());
            emit propertyChanged(row, row);
        }
        break;
    default:
        break;
    }
}

void QMetaPropertyAdaptor::propertyUpdated()
{
    Q_ASSERT(senderSignalIndex() >= 0);
    if (m_notifyGuard) // do not emit change notifications during reading (happens for eg. lazy computed properties like QQItem::childrenRect, that confuses the hell out of QSFPM)
        return;

    const int row = m_notifyToRowMap.value(senderSignalIndex());
    emit propertyChanged(row, row);
}
