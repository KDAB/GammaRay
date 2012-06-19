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

#ifndef GAMMARAY_OBJECTVISUALIZER_VTKPANEL_H
#define GAMMARAY_OBJECTVISUALIZER_VTKPANEL_H

#include <QToolBar>

class QComboBox;

namespace GammaRay {

class VtkWidget;

class VtkPanel : public QToolBar
{
  Q_OBJECT

  public:
    explicit VtkPanel(VtkWidget *vtkWidget, QWidget *parent = 0);
    virtual ~VtkPanel();

  public slots:
     void layoutChanged(int);
     void stereoModeChanged(int);

  private:
    VtkWidget *m_vtkWidget;

    QComboBox *m_layoutBox;
    QComboBox *m_stereoBox;
    QString m_currentLayout;
};

}

#endif // GAMMARAY_VTKPANEL_H
