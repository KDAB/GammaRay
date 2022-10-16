/*
  qtivisupport.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qtivisupport.h"
#ifdef QTIVI_DISABLE_IVIPROPERTIES
#include "qtiviobjectmodel.h"
#else
#include "qtivipropertymodel.h"
#endif

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probe.h>

#include <QIviAbstractFeature>
#include <QIviServiceObject>
#include <QIviZonedFeatureInterface>

#include <QDebug>

using namespace GammaRay;

static void registerMetaTypes()
{
    qRegisterMetaType<QIviServiceObject *>();

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QIviServiceInterface);
    MO_ADD_PROPERTY_RO(QIviServiceInterface, interfaces);

    MO_ADD_METAOBJECT2(QIviServiceObject, QObject, QIviServiceInterface);

    MO_ADD_METAOBJECT1(QIviAbstractFeature, QObject);
    MO_ADD_PROPERTY_RO(QIviAbstractFeature, error);

    MO_ADD_METAOBJECT1(QIviZonedFeatureInterface, QObject);
    MO_ADD_PROPERTY_RO(QIviZonedFeatureInterface, availableZones);
}

QtIviSupport::QtIviSupport(Probe *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);
    registerMetaTypes();

#ifdef QTIVI_DISABLE_IVIPROPERTIES
    auto propertyModel = new QtIviObjectModel(Probe::instance());
#else
    auto propertyModel = new QtIviPropertyModel(Probe::instance());
#endif

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.PropertyModel"), propertyModel);
    // m_selectionModel = ObjectBroker::selectionModel(filterModel);
}
