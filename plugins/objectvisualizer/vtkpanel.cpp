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

#include <vtkAssignCoordinatesLayoutStrategy.h>
#include <vtkAttributeClustering2DLayoutStrategy.h>
#include <vtkCircularLayoutStrategy.h>
#include <vtkClustering2DLayoutStrategy.h>
#include <vtkCommunity2DLayoutStrategy.h>
#include <vtkConeLayoutStrategy.h>
#include <vtkConstrained2DLayoutStrategy.h>
#include <vtkCosmicTreeLayoutStrategy.h>
#include <vtkFast2DLayoutStrategy.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkRandomLayoutStrategy.h>
#include <vtkRenderWindow.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkSimple3DCirclesStrategy.h>
#include <vtkSpanTreeLayoutStrategy.h>
#include <vtkTreeLayoutStrategy.h>
#include <vtkTreeOrbitLayoutStrategy.h>

using namespace GammaRay;

VtkPanel::VtkPanel(VtkWidget *vtkWidget, QWidget *parent)
    : QToolBar(parent), m_vtkWidget(vtkWidget),
      m_currentLayoutStrategy(LayoutStrategy::SpanTree) {
    addWidget(new QLabel(tr("Layout:")));
    m_layoutBox = new QComboBox;

    m_layoutBox->addItem(tr("Span Tree"),
                         QVariant::fromValue(LayoutStrategy::SpanTree));
    m_layoutBox->addItem(tr("Force Directed 2D"),
                         QVariant::fromValue(LayoutStrategy::ForceDirected2D));
    m_layoutBox->addItem(tr("Force Directed 3D"),
                         QVariant::fromValue(LayoutStrategy::ForceDirected3D));
    m_layoutBox->addItem(tr("Simple 2D"),
                         QVariant::fromValue(LayoutStrategy::Simple2D));
    m_layoutBox->addItem(tr("Clustering 2D"),
                         QVariant::fromValue(LayoutStrategy::Clustering2D));
    m_layoutBox->addItem(tr("Community 2D"),
                         QVariant::fromValue(LayoutStrategy::Community2D));
    m_layoutBox->addItem(tr("Fast 2D"),
                         QVariant::fromValue(LayoutStrategy::Fast2D));
    m_layoutBox->addItem(tr("Circular"),
                         QVariant::fromValue(LayoutStrategy::Circular));
    m_layoutBox->addItem(tr("Tree"), QVariant::fromValue(LayoutStrategy::Tree));
    //    m_layoutBox->addItem(tr("Cosmic Tree"),
    //                         QVariant::fromValue(LayoutStrategy::CosmicTree));
    m_layoutBox->addItem(tr("Cone"), QVariant::fromValue(LayoutStrategy::Cone));
    m_layoutBox->addItem(tr("Random"),
                         QVariant::fromValue(LayoutStrategy::Random));
    m_layoutBox->addItem(
        tr("Assign Coordinates"),
        QVariant::fromValue(LayoutStrategy::AssignCoordinates));
    m_layoutBox->addItem(
        tr("Attribute Clustering 2D"),
        QVariant::fromValue(LayoutStrategy::AttributeClustering2D));
    m_layoutBox->addItem(tr("Constrained 2D"),
                         QVariant::fromValue(LayoutStrategy::Constrained2D));
    m_layoutBox->addItem(tr("Simple 3D Circles"),
                         QVariant::fromValue(LayoutStrategy::Simple3DCircles));
    //    m_layoutBox->addItem(tr("Tree Orbit"),
    //                         QVariant::fromValue(LayoutStrategy::TreeOrbit));

    connect(m_layoutBox, SIGNAL(currentIndexChanged(int)),
            SLOT(layoutChanged(int)));
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

static vtkGraphLayoutStrategy *
layoutStrategy(VtkPanel::LayoutStrategy strategy) {
    switch (strategy) {
    case VtkPanel::LayoutStrategy::Cone:
        return vtkConeLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Tree: {
        vtkTreeLayoutStrategy *strategy = vtkTreeLayoutStrategy::New();
        strategy->SetRadial(true);
        return strategy;
    }
    case VtkPanel::LayoutStrategy::Fast2D:
        return vtkFast2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Random:
        return vtkRandomLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Circular:
        return vtkCircularLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Simple2D:
        return vtkSimple2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::SpanTree:
        return vtkSpanTreeLayoutStrategy::New();
        //    case VtkPanel::LayoutStrategy::CosmicTree:
        //        return vtkCosmicTreeLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Community2D:
        return vtkCommunity2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Clustering2D:
        return vtkClustering2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::ForceDirected2D:
        return vtkForceDirectedLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::ForceDirected3D: {
        vtkForceDirectedLayoutStrategy *strategy =
            vtkForceDirectedLayoutStrategy::New();
        strategy->SetThreeDimensionalLayout(true);
        return strategy;
    }
    case VtkPanel::LayoutStrategy::AssignCoordinates:
        return vtkAssignCoordinatesLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::AttributeClustering2D:
        return vtkAttributeClustering2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Constrained2D:
        return vtkConstrained2DLayoutStrategy::New();
    case VtkPanel::LayoutStrategy::Simple3DCircles:
        return vtkSimple3DCirclesStrategy::New();
        //    case VtkPanel::LayoutStrategy::TreeOrbit:
        //        return vtkTreeOrbitLayoutStrategy::New();
    }
    return {};
}

void VtkPanel::layoutChanged(int index) {
    const auto layout = m_layoutBox->itemData(index).value<LayoutStrategy>();
    if (m_currentLayoutStrategy == layout)
        return;

    // update
    vtkGraphLayoutStrategy *strategy = layoutStrategy(layout);
    m_vtkWidget->layoutView()->SetLayoutStrategy(strategy);
    m_vtkWidget->layoutView()->ResetCamera();
    m_vtkWidget->layoutView()->Render();
    m_vtkWidget->GetInteractor()->Start();
    m_currentLayoutStrategy = layout;
}

void VtkPanel::stereoModeChanged(int index) {
    const int stereoMode = m_stereoBox->itemData(index).toInt();
    if (stereoMode <= 0) {
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoRender(false);
    } else {
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoRender(true);
        m_vtkWidget->layoutView()->GetRenderWindow()->SetStereoType(stereoMode);
    }
    m_vtkWidget->layoutView()->GetRenderWindow()->StereoUpdate();
}

VtkPanel::~VtkPanel() {}
