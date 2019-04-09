/*
  3dinspector.h

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

#ifndef GAMMARAY_3DINSPECTOR_3DINSPECTOR_H
#define GAMMARAY_3DINSPECTOR_3DINSPECTOR_H

#include "qt3dinspectorinterface.h"

#include <core/toolfactory.h>

#include <Qt3DCore/QNode>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QItemSelectionModel;
namespace Qt3DCore {
class QAspectEngine;
class QEntity;
}
namespace Qt3DRender {
class QFrameGraphNode;
}
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;
class Qt3DEntityTreeModel;
class FrameGraphModel;

class Qt3DInspector : public Qt3DInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::Qt3DInspectorInterface)
public:
    explicit Qt3DInspector(Probe *probe, QObject *parent = nullptr);
    ~Qt3DInspector();

public slots:
    void selectEngine(int row) override;

private slots:
    void objectSelected(QObject *obj);

private:
    void entitySelectionChanged(const QItemSelection &selection);
    void selectEngine(Qt3DCore::QAspectEngine *engine);
    void selectEntity(Qt3DCore::QEntity *entity);
    void frameGraphSelectionChanged(const QItemSelection &selection);
    void selectFrameGraphNode(Qt3DRender::QFrameGraphNode *node);

    void registerCoreMetaTypes();
    void registerInputMetaTypes();
    void registerRenderMetaTypes();
    void registerAnimationMetaTypes();
    void registerExtensions();

private:
    QAbstractItemModel *m_engineModel;
    Qt3DCore::QAspectEngine *m_engine;

    Qt3DEntityTreeModel *m_entityModel;
    QItemSelectionModel *m_entitySelectionModel;
    Qt3DCore::QEntity *m_currentEntity;
    PropertyController *m_entitryPropertyController;

    FrameGraphModel *m_frameGraphModel;
    QItemSelectionModel *m_frameGraphSelectionModel;
    Qt3DRender::QFrameGraphNode *m_currentFrameGraphNode;
    PropertyController *m_frameGraphPropertyController;
};

class Qt3DInspectorFactory : public QObject,
    public StandardToolFactory<Qt3DCore::QNode, Qt3DInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_3dinspector.json")

public:
    explicit Qt3DInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
