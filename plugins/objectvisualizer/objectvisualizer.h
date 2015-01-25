/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/toolfactory.h>

namespace GammaRay {

class GraphViewer : public QObject
{
  Q_OBJECT

  public:
    explicit GraphViewer(ProbeInterface *probe, QObject *parent = 0);
    ~GraphViewer();
};

class GraphViewerFactory : public QObject, public StandardToolFactory<QObject, GraphViewer>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.GraphViewer")

  public:
    explicit GraphViewerFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Object Visualizer");
    }
};

}

#endif // GAMMARAY_GRAPHVIEWER_H
