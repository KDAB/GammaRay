/*
  wlcompositorinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WLCOMPOSITORINSPECTOR_WLCOMPOSITORINSPECTOR_H
#define GAMMARAY_WLCOMPOSITORINSPECTOR_WLCOMPOSITORINSPECTOR_H

#include <core/toolfactory.h>

#include <QWaylandCompositor>
#include <QStringList>

#include "wlcompositorinterface.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

struct wl_client;

namespace GammaRay {

class ClientsModel;
class Logger;
class ResourcesModel;
class SurfaceView;

class WlCompositorInspector : public WlCompositorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::WlCompositorInterface)
public:
    explicit WlCompositorInspector(Probe *probe, QObject *parent = nullptr);
    ~WlCompositorInspector() override;

public slots:
    void connected() override;
    void disconnected() override;
    void setSelectedClient(int index) override;
    void setSelectedResource(uint id) override;

private slots:
    void objectAdded(QObject *obj);
    void objectSelected(QObject *obj);

private:
    void init(QWaylandCompositor *compositor);
    void addClient(wl_client *c);

    QWaylandCompositor *m_compositor;
    ClientsModel *m_clientsModel;
    QItemSelectionModel *m_clientSelectionModel;
    Logger *m_logger;
    ResourcesModel *m_resourcesModel;
    SurfaceView *m_surfaceView;
};

class WlCompositorInspectorFactory : public QObject, public StandardToolFactory<QWaylandCompositor, WlCompositorInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_wlcompositorinspector.json")

public:
    explicit WlCompositorInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};

}

#endif
