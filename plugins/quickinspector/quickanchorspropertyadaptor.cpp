/*
  quickanchorspropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "quickanchorspropertyadaptor.h"

#include <core/propertydata.h>
#include <core/util.h>

#include <private/qquickitem_p.h>

#include <QDebug>

using namespace GammaRay;

QuickAnchorsPropertyAdaptor::QuickAnchorsPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
    , m_anchorsPropertyIndex(-1)
    , m_notifyGuard(false)
{
}

QuickAnchorsPropertyAdaptor::~QuickAnchorsPropertyAdaptor() = default;

void QuickAnchorsPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    m_anchorsPropertyIndex = -1;
    auto mo = oi.metaObject();
    if (!mo || oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return;

    int propertyIndex = mo->indexOfProperty("anchors");
    if (propertyIndex == -1)
        return;

    const auto prop = mo->property(propertyIndex);
    if (QString::compare(prop.typeName(), "QQuickAnchors*") != 0)
        return;

    m_anchorsPropertyIndex = propertyIndex;
}

int QuickAnchorsPropertyAdaptor::count() const
{
    if (!object().isValid())
        return 0;

    return m_anchorsPropertyIndex == -1 ? 0 : 1;
}

PropertyData QuickAnchorsPropertyAdaptor::propertyData(int index) const
{
    Q_ASSERT(index == 0);

    PropertyData data;
    if (!object().isValid())
        return data;

    m_notifyGuard = true;
    const auto mo = object().metaObject();
    Q_ASSERT(mo);

    const auto prop = mo->property(m_anchorsPropertyIndex);

    data.setName(prop.name());
    data.setTypeName(prop.typeName());

    auto pmo = mo;
    while (pmo->propertyOffset() > m_anchorsPropertyIndex)
        pmo = pmo->superClass();
    data.setClassName(pmo->className());

    data.setValue(QVariant::fromValue(QQuickItemPrivate::get(qobject_cast<QQuickItem *>(object().qtObject()))->_anchors));

    PropertyModel::PropertyFlags f(PropertyModel::None);
    if (prop.isConstant())
        f |= PropertyModel::Constant;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (prop.isDesignable(object().qtObject()))
#else
    if (prop.isDesignable())
#endif
        f |= PropertyModel::Designable;
    if (prop.isFinal())
        f |= PropertyModel::Final;
    if (prop.isResettable())
        f |= PropertyModel::Resetable;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (prop.isScriptable(object().qtObject()))
#else
    if (prop.isScriptable())
#endif
        f |= PropertyModel::Scriptable;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (prop.isStored(object().qtObject()))
#else
    if (prop.isStored())
#endif
        f |= PropertyModel::Stored;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (prop.isUser(object().qtObject()))
#else
    if (prop.isUser())
#endif
        f |= PropertyModel::User;
    if (prop.isWritable())
        f |= PropertyModel::Writable;
    data.setPropertyFlags(f);
    data.setRevision(prop.revision());
    if (prop.hasNotifySignal())
        data.setNotifySignal(Util::prettyMethodSignature(prop.notifySignal()));

    data.setAccessFlags(PropertyData::Readable);

    m_notifyGuard = false;
    return data;
}

QuickAnchorsPropertyAdaptorFactory *QuickAnchorsPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QuickAnchorsPropertyAdaptorFactory::create(const ObjectInstance &oi,
                                                            QObject *parent) const
{
    if (oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return nullptr;

    if (qobject_cast<QQuickItem *>(oi.qtObject()))
        return new QuickAnchorsPropertyAdaptor(parent);

    return nullptr;
}

QuickAnchorsPropertyAdaptorFactory *QuickAnchorsPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QuickAnchorsPropertyAdaptorFactory;
    return s_instance;
}
