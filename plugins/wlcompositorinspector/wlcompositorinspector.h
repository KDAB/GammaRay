/*
  wlcompositorinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

class WlCompositorInspector: public WlCompositorInterface
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

class WlCompositorInspectorFactory: public QObject, public StandardToolFactory<QWaylandCompositor, WlCompositorInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_wlcompositorinspector.json")

public:
    explicit WlCompositorInspectorFactory(QObject *parent = nullptr) : QObject(parent)
    {
    }
};

}

#endif

