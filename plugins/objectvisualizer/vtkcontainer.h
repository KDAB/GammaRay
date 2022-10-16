/*
  vtkcontainer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
