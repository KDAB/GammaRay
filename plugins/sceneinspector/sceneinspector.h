/*
  sceneinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    static void registerGraphicsViewMetaTypes();
    static void registerVariantHandlers();
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
