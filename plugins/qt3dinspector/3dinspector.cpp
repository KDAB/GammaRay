/*
  3dinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "3dinspector.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>

#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QEntity>

using namespace GammaRay;

Qt3DInspector::Qt3DInspector(ProbeInterface* probe, QObject* parent) :
    QObject(parent)
{
    Q_UNUSED(probe);

    registerCoreMetaTypes();
    registerRenderMetaTypes();
}

Qt3DInspector::~Qt3DInspector()
{
}

void Qt3DInspector::registerCoreMetaTypes()
{
    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(Qt3DCore::QNode, QObject);
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, bool, notificationsBlocked);
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, Qt3DCore::QNodeList, childrenNodes);

    MO_ADD_METAOBJECT1(Qt3DCore::QComponent, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QComponent, QVector<Qt3DCore::QEntity*>, entities);

    MO_ADD_METAOBJECT1(Qt3DCore::QEntity, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QComponentList, components);
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QEntity*, parentEntity);
}

void GammaRay::Qt3DInspector::registerRenderMetaTypes()
{
    qRegisterMetaType<Qt3DRender::QFrameGraphNode*>();
}

QString Qt3DInspectorFactory::name() const
{
  return tr("Qt3D Inspector");
}
