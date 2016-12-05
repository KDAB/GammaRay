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
#include "framegraphmodel.h"
#include "geometryextension/qt3dgeometryextension.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/propertycontroller.h>
#include <core/remote/serverproxymodel.h>

#include <common/modelevent.h>
#include <common/objectbroker.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderSettings>

#include <Qt3DInput/QAbstractPhysicalDevice>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QEntity>

#include <QDebug>
#include <QItemSelection>
#include <QItemSelectionModel>

using namespace GammaRay;

Qt3DInspector::Qt3DInspector(ProbeInterface *probe, QObject *parent)
    : Qt3DInspectorInterface(parent)
    , m_engine(nullptr)
    , m_entityModel(new Qt3DEntityTreeModel(this))
    , m_currentEntity(nullptr)
    , m_entitryPropertyController(new PropertyController(QStringLiteral(
                                                             "com.kdab.GammaRay.Qt3DInspector.entityPropertyController"),
                                                         this))
    , m_frameGraphModel(new FrameGraphModel(this))
    , m_currentFrameGraphNode(nullptr)
    , m_frameGraphPropertyController(new PropertyController(QStringLiteral(
                                                                "com.kdab.GammaRay.Qt3DInspector.frameGraphPropertyController"),
                                                            this))
{
    registerCoreMetaTypes();
    registerInputMetaTypes();
    registerRenderMetaTypes();
    registerExtensions();

    auto engineFilterModel = new ObjectTypeFilterProxyModel<Qt3DCore::QAspectEngine>(this);
    engineFilterModel->setSourceModel(probe->objectListModel());
    auto proxy = new SingleColumnObjectProxyModel(this);
    proxy->setSourceModel(engineFilterModel);
    m_engineModel = proxy;
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.Qt3DInspector.engineModel"), m_engineModel);

    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), m_entityModel,
            SLOT(objectCreated(QObject*)));
    connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)), m_entityModel,
            SLOT(objectDestroyed(QObject*)));
    connect(probe->probe(), SIGNAL(objectReparented(QObject*)), m_entityModel,
            SLOT(objectReparented(QObject*)));
    auto entityProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    entityProxy->setSourceModel(m_entityModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.sceneModel"), entityProxy);
    m_entitySelectionModel = ObjectBroker::selectionModel(entityProxy);
    connect(m_entitySelectionModel, &QItemSelectionModel::selectionChanged, this,
            &Qt3DInspector::entitySelectionChanged);

    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), m_frameGraphModel, SLOT(objectCreated(QObject*)));
    connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)), m_frameGraphModel, SLOT(objectDestroyed(QObject*)));
    connect(probe->probe(), SIGNAL(objectReparented(QObject*)), m_frameGraphModel, SLOT(objectReparented(QObject*)));
    auto frameGraphProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    frameGraphProxy->setSourceModel(m_frameGraphModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.Qt3DInspector.frameGraphModel"), frameGraphProxy);
    m_frameGraphSelectionModel = ObjectBroker::selectionModel(frameGraphProxy);
    connect(m_frameGraphSelectionModel, &QItemSelectionModel::selectionChanged, this,
            &Qt3DInspector::frameGraphSelectionChanged);

    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)), this,
            SLOT(objectSelected(QObject*)));
}

Qt3DInspector::~Qt3DInspector()
{
}

void Qt3DInspector::selectEngine(int row)
{
    Qt3DCore::QAspectEngine *engine = nullptr;
    const auto idx = m_engineModel->index(row, 0);
    if (idx.isValid())
        engine
            = qobject_cast<Qt3DCore::QAspectEngine *>(idx.data(
                                                          ObjectModel::ObjectRole).value<QObject *>());

    selectEngine(engine);
}

void Qt3DInspector::selectEngine(Qt3DCore::QAspectEngine *engine)
{
    if (m_engine == engine)
        return;
    m_engine = engine;
    m_entityModel->setEngine(engine);
    if (!engine)
        return;

    // TODO watch for changes
    auto rootEntity = engine->rootEntity();
    if (!rootEntity)
        return;
    foreach (auto component, rootEntity->components()) {
        if (auto renderSettings = qobject_cast<Qt3DRender::QRenderSettings *>(component)) {
            m_frameGraphModel->setRenderSettings(renderSettings);
            break;
        }
    }
}

void Qt3DInspector::entitySelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    const auto index = selection.first().topLeft();
    auto entity = index.data(ObjectModel::ObjectRole).value<Qt3DCore::QEntity *>();
    selectEntity(entity);
}

void Qt3DInspector::selectEntity(Qt3DCore::QEntity *entity)
{
    if (m_currentEntity == entity)
        return;
    m_currentEntity = entity;
    m_entitryPropertyController->setObject(entity);

    // update selelction if we got here via object navigation
    const auto model = m_entitySelectionModel->model();
    Model::used(model);

    const auto indexList = model->match(model->index(0,
                                                     0), ObjectModel::ObjectRole,
                                        QVariant::fromValue<Qt3DCore::QEntity *>(
                                            entity), 1,
                                        Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;
    const auto index = indexList.first();
    m_entitySelectionModel->select(index,
                                   QItemSelectionModel::Select | QItemSelectionModel::Clear | QItemSelectionModel::Rows
                                   | QItemSelectionModel::Current);
}

void Qt3DInspector::frameGraphSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    const auto index = selection.first().topLeft();
    auto node = index.data(ObjectModel::ObjectRole).value<Qt3DRender::QFrameGraphNode *>();
    selectFrameGraphNode(node);
}

void Qt3DInspector::selectFrameGraphNode(Qt3DRender::QFrameGraphNode *node)
{
    if (m_currentFrameGraphNode == node)
        return;
    m_currentFrameGraphNode = node;
    m_frameGraphPropertyController->setObject(node);

    // update selelction if we got here via object navigation
    const auto model = m_frameGraphSelectionModel->model();
    Model::used(model);

    const auto indexList = model->match(model->index(0,
                                                     0), ObjectModel::ObjectRole,
                                        QVariant::fromValue<Qt3DRender::QFrameGraphNode *>(
                                            node), 1,
                                        Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;
    const auto index = indexList.first();
    m_frameGraphSelectionModel->select(index,
                                       QItemSelectionModel::Select | QItemSelectionModel::Clear | QItemSelectionModel::Rows
                                       | QItemSelectionModel::Current);
}

void Qt3DInspector::objectSelected(QObject *obj)
{
    if (auto engine = qobject_cast<Qt3DCore::QAspectEngine *>(obj))
        selectEngine(engine);
    // TODO check if the engine matches, otherwise switch that too
    else if (auto entity = qobject_cast<Qt3DCore::QEntity *>(obj))
        selectEntity(entity);
    else if (auto node = qobject_cast<Qt3DRender::QFrameGraphNode *>(obj))
        selectFrameGraphNode(node);
}

void Qt3DInspector::registerCoreMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(Qt3DCore::QNode, QObject);
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, bool, notificationsBlocked);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, Qt3DCore::QNodeList, childrenNodes);
#else
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, Qt3DCore::QNodeVector, childNodes);
#endif

    MO_ADD_METAOBJECT1(Qt3DCore::QComponent, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QComponent, QVector<Qt3DCore::QEntity *>, entities);

    MO_ADD_METAOBJECT1(Qt3DCore::QEntity, Qt3DCore::QNode);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QComponentList, components);
#else
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QComponentVector, components);
#endif
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, Qt3DCore::QEntity *, parentEntity);
}

void Qt3DInspector::registerInputMetaTypes()
{
    qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>();
}

void Qt3DInspector::registerRenderMetaTypes()
{
    qRegisterMetaType<Qt3DRender::QAttribute *>();
    qRegisterMetaType<Qt3DRender::QBuffer *>();
    qRegisterMetaType<Qt3DRender::QCamera *>();
    qRegisterMetaType<Qt3DRender::QEffect *>();
    qRegisterMetaType<Qt3DRender::QFrameGraphNode *>();
    qRegisterMetaType<Qt3DRender::QGraphicsApiFilter *>();
}

void Qt3DInspector::registerExtensions()
{
    PropertyController::registerExtension<Qt3DGeometryExtension>();
}
