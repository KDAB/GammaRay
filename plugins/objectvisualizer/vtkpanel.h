/*
  vtkpanel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    explicit VtkPanel(VtkWidget *vtkWidget, QWidget *parent = nullptr);
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
