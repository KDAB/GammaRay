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

#include "vtkpanel.h"
#include "vtkwidget.h"

#include <QComboBox>
#include <QDebug>
#include <QLabel>

#include <vtkGraphLayoutView.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkRenderWindow.h>
#include <vtkSpanTreeLayoutStrategy.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkTreeLayoutStrategy.h>

using namespace GammaRay;

VtkPanel::VtkPanel(VtkWidget *vtkWidget, QWidget *parent)
    : QToolBar(parent)
    , m_vtkWidget(vtkWidget)
    , m_currentLayout("spanTree")
{
    addWidget(new QLabel(tr("Layout:")));
    m_layoutBox = new QComboBox;
#if 0
    m_layoutBox->addItem(tr("Tree Layout"), "tree");
#endif

    m_layoutBox->addItem(tr("Span Tree Layout"), "spanTree");
    m_layoutBox->addItem(tr("Force Directed Layout"), "forceDirected");
    m_layoutBox->addItem(tr("Force Directed Layout (3D)"), "forceDirected3D");
    m_layoutBox->addItem(tr("Simple 2D Layout"), "simple2D");
    connect(m_layoutBox, SIGNAL(currentIndexChanged(int)), SLOT(layoutChanged(int)));
    addWidget(m_layoutBox);

    addWidget(new QLabel(tr("Stereo:")));
    m_stereoBox = new QComboBox;
    m_stereoBox->addItem(tr("Off"), 0);
    m_stereoBox->addItem(tr("Crystal Eyes"), VTK_STEREO_CRYSTAL_EYES);
    m_stereoBox->addItem(tr("Red/Blue"), VTK_STEREO_RED_BLUE);
    m_stereoBox->addItem(tr("Interlaced"), VTK_STEREO_INTERLACED);
    m_stereoBox->addItem(tr("Left"), VTK_STEREO_LEFT);
    m_stereoBox->addItem(tr("Right"), VTK_STEREO_RIGHT);
    m_stereoBox->addItem(tr("Dresden"), VTK_STEREO_DRESDEN);
    m_stereoBox->addItem(tr("Anaglyph"), VTK_STEREO_ANAGLYPH);
    m_stereoBox->addItem(tr("Checkboard"), VTK_STEREO_CHECKERBOARD);
    connect(m_stereoBox, SIGNAL(currentIndexChanged(int)), SLOT(stereoModeChanged(int)));
    addWidget(m_stereoBox);
}

static vtkGraphLayoutStrategy *layoutStrategyForName(const QString &layoutName)
{
    if (layoutName == "tree") {
        vtkTreeLayoutStrategy *strategy = vtkTreeLayoutStrategy::New();
        strategy->SetRadial(true);
        return strategy;
    } else if (layoutName == "spanTree") {
        return vtkSpanTreeLayoutStrategy::New();
    } else if (layoutName == "forceDirected") {
        return vtkForceDirectedLayoutStrategy::New();
    } else if (layoutName == "forceDirected3D") {
        vtkForceDirectedLayoutStrategy *strategy = vtkForceDirectedLayoutStrategy::New();
        strategy->SetThreeDimensionalLayout(true);
        return strategy;
    } else if (layoutName == "simple2D") {
        return vtkSimple2DLayoutStrategy::New();
    } else {
        return nullptr;
    }
}

void VtkPanel::layoutChanged(int index)
{
    const QString layoutName = m_layoutBox->itemData(index).toString();
    if (m_currentLayout == layoutName)
        return;

    // update
    vtkGraphLayoutStrategy *strategy = layoutStrategyForName(layoutName);
    m_vtkWidget->layoutView()->SetLayoutStrategy(strategy);
    m_vtkWidget->layoutView()->ResetCamera();
    m_vtkWidget->layoutView()->Render();
    m_vtkWidget->GetInteractor()->Start();
    m_currentLayout = layoutName;
}

void VtkPanel::stereoModeChanged(int index)
{
    const int stereoMode = m_stereoBox->itemData(index).toInt();
    if (stereoMode <= 0) {
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoRender(false);
    } else {
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoRender(true);
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoType(stereoMode);
    }
    m_vtkWidget->layoutView()->GetRenderWindow()->StereoUpdate();
}

VtkPanel::~VtkPanel()
{
}
