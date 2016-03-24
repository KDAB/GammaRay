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
#include "qt3dentitytreemodel.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>

#include <Qt3DRender/QFrameGraphNode>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QEntity>

#include <QDebug>

using namespace GammaRay;

Qt3DInspector::Qt3DInspector(ProbeInterface* probe, QObject* parent) :
    Qt3DInspectorInterface(parent),
    m_engine(nullptr),
    m_entityModel(new Qt3DEntityTreeModel(this))
{
    registerCoreMetaTypes();
    registerRenderMetaTypes();

    auto engineFilterModel = new ObjectTypeFilterProxyModel<Qt3DCore::QAspectEngine>(this);
    engineFilterModel->setSourceModel(probe->objectListModel());
    auto proxy = new SingleColumnObjectProxyModel(this);
    proxy->setSourceModel(engineFilterModel);
    m_engineModel = proxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.engineModel"), m_engineModel);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.sceneModel"), m_entityModel);

    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)), this, SLOT(objectSelected(QObject*)));
}

Qt3DInspector::~Qt3DInspector()
{
}

void Qt3DInspector::selectEngine(int row)
{
    Qt3DCore::QAspectEngine* engine = nullptr;
    const auto idx = m_engineModel->index(row, 0);
    if (idx.isValid())
        engine = qobject_cast<Qt3DCore::QAspectEngine*>(idx.data(ObjectModel::ObjectRole).value<QObject*>());
    selectEngine(engine);
}

void Qt3DInspector::selectEngine(Qt3DCore::QAspectEngine* engine)
{
    if (m_engine == engine)
        return;
    m_engine = engine;
    m_entityModel->setEngine(engine);
}

void Qt3DInspector::objectSelected(QObject* obj)
{
    if (auto engine = qobject_cast<Qt3DCore::QAspectEngine*>(obj))
        selectEngine(engine);
}

void Qt3DInspector::registerCoreMetaTypes()
{
    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(Qt3DCore::QNode, QObject);
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, bool, notificationsBlocked);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, Qt3DCore::QNodeList, childrenNodes);
#else
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, Qt3DCore::QNodeVector, childNodes);
#endif

    MO_ADD_METAOBJECT1(Qt3DCore::QComponent, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QComponent, QVector<Qt3DCore::QEntity*>, entities);

    MO_ADD_METAOBJECT1(Qt3DCore::QEntity, Qt3DCore::QNode);
#if QT_VERSION < QT_VERSION_CHECK(5, 7 , 0)
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QComponentList, components);
#else
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QComponentVector, components);
#endif
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
