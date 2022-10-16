/*
  vtkcontainer.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "vtkcontainer.h"

#include <QVBoxLayout>

#include "vtkwidget.h"
#include "vtkpanel.h"

using namespace GammaRay;

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);

    m_vtkWidget = new VtkWidget(this);

    m_vtkPanel = new VtkPanel(m_vtkWidget, this);
    vbox->addWidget(m_vtkPanel);
    vbox->addWidget(m_vtkWidget);
}

GraphWidget::~GraphWidget()
{
}
