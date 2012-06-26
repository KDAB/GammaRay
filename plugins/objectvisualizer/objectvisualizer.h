/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZER_H
#define GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZER_H

#include "include/toolfactory.h"

#include <QWidget>

class QTreeView;
class QModelIndex;

namespace GammaRay {

class GraphWidget;
class ProbeInterface;

class GraphViewer : public QWidget
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.GraphViewer")

  public:
    explicit GraphViewer(ProbeInterface *probe, QWidget *parent = 0);
    virtual ~GraphViewer();

  private Q_SLOTS:
    void delayedInit();
    void handleRowChanged(const QModelIndex &index);

  private:
    QTreeView *mObjectTreeView;
    GraphWidget *mWidget;
    ProbeInterface *mProbeIface;
};

class GraphViewerFactory : public QObject, public StandardToolFactory<QObject, GraphViewer>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)

  public:
    explicit GraphViewerFactory(QObject *parent = 0) : QObject(parent) {}

    virtual inline QString name() const
    {
      return tr("Object Visualizer");
    }
};

}

#endif // GAMMARAY_GRAPHVIEWER_H
