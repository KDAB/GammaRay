/*
  sceneinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEINSPECTOR_H
#define GAMMARAY_SCENEINSPECTOR_SCENEINSPECTOR_H

#include <core/toolfactory.h>
#include "sceneinspectorinterface.h"

#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;
class SceneModel;

class SceneInspector : public SceneInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::SceneInspectorInterface)
public:
    explicit SceneInspector(Probe *probe, QObject *parent = nullptr);

private slots:
    void initializeGui() override;
    void renderScene(const QTransform &transform, const QSize &size) override;

    void sceneSelected(const QItemSelection &selection);
    void sceneItemSelectionChanged(const QItemSelection &selection);
    void sceneItemSelected(QGraphicsItem *item);
    void qObjectSelected(QObject *object, const QPoint &pos);
    void nonQObjectSelected(void *obj, const QString &typeName);
    void sceneClicked(const QPointF &pos) override;

    void clientConnectedChanged(bool clientConnected);

private:
    QString findBestType(QGraphicsItem *item);
    void registerGraphicsViewMetaTypes();
    void registerVariantHandlers();
    void connectToScene();

private:
    SceneModel *m_sceneModel;
    QItemSelectionModel *m_itemSelectionModel;
    PropertyController *m_propertyController;
    bool m_clientConnected;
};

class SceneInspectorFactory : public QObject,
    public StandardToolFactory<QGraphicsScene, SceneInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_sceneinspector.json")
public:
    explicit SceneInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_SCENEINSPECTOR_H
