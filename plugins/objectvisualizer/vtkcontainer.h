/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTVISUALIZER_VTKCONTAINER_H
#define GAMMARAY_OBJECTVISUALIZER_VTKCONTAINER_H

#include <QWidget>

namespace GammaRay {
class VtkPanel;
class VtkWidget;

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    virtual ~GraphWidget();

    VtkWidget *vtkWidget() const
    {
        return m_vtkWidget;
    }

    VtkPanel *vtkPanel() const
    {
        return m_vtkPanel;
    }

private:
    VtkWidget *m_vtkWidget;
    VtkPanel *m_vtkPanel;
};
}

#endif // GAMMARAY_GRAPHWIDGET_H
