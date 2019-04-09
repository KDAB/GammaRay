/*
  3dinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "paintanalyzerextension/qt3dpaintedtextureanalyzerextension.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/propertycontroller.h>
#include <core/remote/serverproxymodel.h>
#include <core/varianthandler.h>

#include <common/modelevent.h>
#include <common/objectbroker.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTextureWrapMode>

#include <Qt3DInput/QAbstractPhysicalDevice>

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
#include <Qt3DAnimation/QAnimationClipData>
#include <Qt3DAnimation/QAnimationController>
#include <Qt3DAnimation/QAnimationGroup>
#include <Qt3DAnimation/QChannelMapper>
#include <Qt3DAnimation/QChannelMapping>
#endif

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QEntity>

#include <QDebug>
#include <QItemSelection>
#include <QItemSelectionModel>

using namespace GammaRay;

Qt3DInspector::Qt3DInspector(Probe *probe, QObject *parent)
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
    registerAnimationMetaTypes();
    registerExtensions();

    auto engineFilterModel = new ObjectTypeFilterProxyModel<Qt3DCore::QAspectEngine>(this);
    engineFilterModel->setSourceModel(probe->objectListModel());
    auto proxy = new SingleColumnObjectProxyModel(this);
    proxy->setSourceModel(engineFilterModel);
    m_engineModel = proxy;
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.Qt3DInspector.engineModel"), m_engineModel);

    connect(probe, &Probe::objectCreated, m_entityModel, &Qt3DEntityTreeModel::objectCreated);
    connect(probe, &Probe::objectDestroyed, m_entityModel, &Qt3DEntityTreeModel::objectDestroyed);
    connect(probe, &Probe::objectReparented, m_entityModel, &Qt3DEntityTreeModel::objectReparented);
    auto entityProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    entityProxy->setSourceModel(m_entityModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.sceneModel"), entityProxy);
    m_entitySelectionModel = ObjectBroker::selectionModel(entityProxy);
    connect(m_entitySelectionModel, &QItemSelectionModel::selectionChanged, this,
            &Qt3DInspector::entitySelectionChanged);

    connect(probe, &Probe::objectCreated, m_frameGraphModel, &FrameGraphModel::objectCreated);
    connect(probe, &Probe::objectDestroyed, m_frameGraphModel, &FrameGraphModel::objectDestroyed);
    connect(probe, &Probe::objectReparented, m_frameGraphModel, &FrameGraphModel::objectReparented);
    auto frameGraphProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    frameGraphProxy->setSourceModel(m_frameGraphModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.Qt3DInspector.frameGraphModel"), frameGraphProxy);
    m_frameGraphSelectionModel = ObjectBroker::selectionModel(frameGraphProxy);
    connect(m_frameGraphSelectionModel, &QItemSelectionModel::selectionChanged, this,
            &Qt3DInspector::frameGraphSelectionChanged);

    connect(probe, &Probe::objectSelected, this, &Qt3DInspector::objectSelected);
}

Qt3DInspector::~Qt3DInspector()
{
}

void Qt3DInspector::selectEngine(int row)
{
    Qt3DCore::QAspectEngine *engine = nullptr;
    const auto idx = m_engineModel->index(row, 0);
    if (idx.isValid()) {
        engine = qobject_cast<Qt3DCore::QAspectEngine *>(
                     idx.data(ObjectModel::ObjectRole).value<QObject *>());
    }

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
                                       QItemSelectionModel::Select |
                                       QItemSelectionModel::Clear |
                                       QItemSelectionModel::Rows |
                                       QItemSelectionModel::Current);
}

void Qt3DInspector::objectSelected(QObject *obj)
{
    if (auto engine = qobject_cast<Qt3DCore::QAspectEngine *>(obj)) {
        selectEngine(engine);
    // TODO check if the engine matches, otherwise switch that too
    } else if (auto entity = qobject_cast<Qt3DCore::QEntity *>(obj)) {
        selectEntity(entity);
    } else if (auto node = qobject_cast<Qt3DRender::QFrameGraphNode *>(obj)) {
        selectFrameGraphNode(node);
    }
}

void Qt3DInspector::registerCoreMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(Qt3DCore::QNode, QObject);
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, notificationsBlocked);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, childrenNodes);
#else
    MO_ADD_PROPERTY_RO(Qt3DCore::QNode, childNodes);
#endif

    MO_ADD_METAOBJECT1(Qt3DCore::QComponent, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QComponent, entities);

    MO_ADD_METAOBJECT1(Qt3DCore::QEntity, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, components);
    MO_ADD_PROPERTY_RO(Qt3DCore::QEntity, parentEntity);
}

void Qt3DInspector::registerInputMetaTypes()
{
    qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>();
}

static QString attributeToString(Qt3DRender::QAttribute *attr)
{
    if (!attr || attr->name().isEmpty())
        return Util::displayString(attr);
    const auto name = ObjectDataProvider::name(attr);
    if (name.isEmpty())
        return attr->name() + QLatin1String(" [") + Util::addressToString(attr) + QLatin1Char(']');
    return Util::displayString(attr);
}

static QString filterKeyToString(Qt3DRender::QFilterKey *key)
{
    if (!key || key->name().isEmpty())
        return Util::displayString(key);
    const auto value = VariantHandler::displayString(key->value());
    if (value.isEmpty())
        return Util::displayString(key);
    return key->name() + QLatin1String(" = ") + value;
}

static QString graphicsApiFilterToString(Qt3DRender::QGraphicsApiFilter *filter)
{
    if (!filter)
        return Util::displayString(filter);

    using namespace Qt3DRender;

    QString s;
    switch (filter->api()) {
        case QGraphicsApiFilter::OpenGLES:
            s = QStringLiteral("OpenGL ES ");
            break;
        case QGraphicsApiFilter::OpenGL:
            s = QStringLiteral("OpenGL ");
            break;
        default:
            return Util::displayString(filter);
    }
    s += QString::fromLatin1("%1.%2").arg(filter->majorVersion()).arg(filter->minorVersion());

    switch (filter->profile()) {
        case QGraphicsApiFilter::NoProfile:
            break;
        case QGraphicsApiFilter::CoreProfile:
            s += QStringLiteral(" core");
            break;
        case QGraphicsApiFilter::CompatibilityProfile:
            s += QStringLiteral(" compat");
            break;
    }

    return s;
}

static QString parameterToString(Qt3DRender::QParameter *parameter)
{
    if (!parameter || parameter->name().isEmpty())
        return Util::displayString(parameter);
    const auto value = VariantHandler::displayString(parameter->value());
    if (value.isEmpty())
        return Util::displayString(parameter);
    return parameter->name() + QLatin1String(" = ") + value;
}

void Qt3DInspector::registerRenderMetaTypes()
{
    qRegisterMetaType<Qt3DRender::QAttribute*>();
    qRegisterMetaType<Qt3DRender::QBuffer*>();
    qRegisterMetaType<Qt3DRender::QCamera*>();
    qRegisterMetaType<Qt3DRender::QEffect*>();
    qRegisterMetaType<Qt3DRender::QFrameGraphNode*>();
    qRegisterMetaType<Qt3DRender::QGraphicsApiFilter*>();
    qRegisterMetaType<Qt3DRender::QTextureWrapMode*>();

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(Qt3DRender::QMaterial, Qt3DCore::QComponent);
    MO_ADD_PROPERTY_RO(Qt3DRender::QMaterial, parameters);

    MO_ADD_METAOBJECT1(Qt3DRender::QEffect, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DRender::QEffect, parameters);
    MO_ADD_PROPERTY_RO(Qt3DRender::QEffect, techniques);

    MO_ADD_METAOBJECT1(Qt3DRender::QGeometry, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DRender::QGeometry, attributes);

    MO_ADD_METAOBJECT1(Qt3DRender::QTechnique, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DRender::QTechnique, filterKeys);
    MO_ADD_PROPERTY_RO(Qt3DRender::QTechnique, parameters);
    MO_ADD_PROPERTY_RO(Qt3DRender::QTechnique, renderPasses);

    MO_ADD_METAOBJECT1(Qt3DRender::QRenderPass, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DRender::QRenderPass, filterKeys);
    MO_ADD_PROPERTY_RO(Qt3DRender::QRenderPass, parameters);
    MO_ADD_PROPERTY_RO(Qt3DRender::QRenderPass, renderStates);

    MO_ADD_METAOBJECT1(Qt3DRender::QAbstractTexture, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DRender::QAbstractTexture, textureImages);

    MO_ADD_METAOBJECT1(Qt3DRender::QSceneLoader, Qt3DCore::QComponent);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY_RO(Qt3DRender::QSceneLoader, entityNames);
#endif

    VariantHandler::registerStringConverter<Qt3DRender::QAttribute*>(attributeToString);
    VariantHandler::registerStringConverter<Qt3DRender::QFilterKey*>(filterKeyToString);
    VariantHandler::registerStringConverter<Qt3DRender::QGraphicsApiFilter*>(graphicsApiFilterToString);
    VariantHandler::registerStringConverter<Qt3DRender::QParameter*>(parameterToString);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
static QString channelMappingToString(Qt3DAnimation::QChannelMapping *mapping)
{
    if (!mapping || mapping->channelName().isEmpty() || mapping->property().isEmpty())
        return Util::displayString(mapping);
    return mapping->channelName() + QLatin1String(" -> ") + Util::displayString(mapping->target())
        + QLatin1Char('.') + mapping->property();
}
#endif

void Qt3DInspector::registerAnimationMetaTypes()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(Qt3DAnimation::QAnimationClipData);
    MO_ADD_PROPERTY_RO(Qt3DAnimation::QAnimationClipData, channelCount);
    MO_ADD_PROPERTY_RO(Qt3DAnimation::QAnimationClipData, isValid);
    MO_ADD_PROPERTY   (Qt3DAnimation::QAnimationClipData, name, setName);

    MO_ADD_METAOBJECT1(Qt3DAnimation::QAnimationController, QObject);
    MO_ADD_PROPERTY_NC(Qt3DAnimation::QAnimationController, animationGroupList);

    MO_ADD_METAOBJECT1(Qt3DAnimation::QAnimationGroup, QObject);
    MO_ADD_PROPERTY_NC(Qt3DAnimation::QAnimationGroup, animationList);

    MO_ADD_METAOBJECT1(Qt3DAnimation::QChannelMapper, Qt3DCore::QNode);
    MO_ADD_PROPERTY_RO(Qt3DAnimation::QChannelMapper, mappings);

    VariantHandler::registerStringConverter<Qt3DAnimation::QChannelMapping*>(channelMappingToString);
#endif
}

void Qt3DInspector::registerExtensions()
{
    PropertyController::registerExtension<Qt3DGeometryExtension>();
    PropertyController::registerExtension<Qt3DPaintedTextureAnalyzerExtension>();
}
