/*
  qtivisupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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
    qRegisterMetaType<QIviServiceObject*>();

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QIviServiceInterface);
    MO_ADD_PROPERTY_RO(QIviServiceInterface, interfaces);

    MO_ADD_METAOBJECT2(QIviServiceObject, QObject, QIviServiceInterface);

    MO_ADD_METAOBJECT1(QIviAbstractFeature, QObject);
    MO_ADD_PROPERTY_RO(QIviAbstractFeature, error);

    MO_ADD_METAOBJECT1(QIviZonedFeatureInterface, QObject);
    MO_ADD_PROPERTY_RO(QIviZonedFeatureInterface, availableZones);
}

QtIviSupport::QtIviSupport(Probe* probe, QObject* parent)
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
    //m_selectionModel = ObjectBroker::selectionModel(filterModel);
}
