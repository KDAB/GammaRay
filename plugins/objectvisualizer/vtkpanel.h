/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_OBJECTVISUALIZER_VTKPANEL_H
#define GAMMARAY_OBJECTVISUALIZER_VTKPANEL_H

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE

namespace GammaRay {
class VtkWidget;

class VtkPanel : public QToolBar
{
    Q_OBJECT

public:
    enum class LayoutStrategy {
        AssignCoordinates,
        AttributeClustering2D,
        Circular,
        Clustering2D,
        Community2D,
        Cone,
        Constrained2D,
        // CosmicTree, // Layout only works on vtkTree unless VTK_USE_BOOST is
        // on
        Fast2D,
        ForceDirected2D,
        ForceDirected3D,
        Random,
        Simple2D,
        Simple3DCircles,
        SpanTree,
        Tree,
        // TreeOrbit // Layout only works on vtkTree unless VTK_USE_BOOST is on
    };
    Q_ENUM(LayoutStrategy)
    explicit VtkPanel(VtkWidget *vtkWidget, QWidget *parent = nullptr);
    virtual ~VtkPanel();

public slots:
    void layoutChanged(int);
    void stereoModeChanged(int);

private:
    VtkWidget *m_vtkWidget;

    QComboBox *m_layoutBox;
    QComboBox *m_stereoBox;
    LayoutStrategy m_currentLayoutStrategy;
};
}

#endif // GAMMARAY_VTKPANEL_H
