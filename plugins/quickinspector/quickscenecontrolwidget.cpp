/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Christoph Sterz <christoph.sterz@kdab.com>

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

#include "quickscenecontrolwidget.h"

#include "quickoverlaylegend.h"
#include "quickscenepreviewwidget.h"
#include "gridsettingswidget.h"

#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

using namespace GammaRay;

QuickSceneControlWidget::QuickSceneControlWidget(QuickInspectorInterface *inspector, QWidget *parent)
    : QWidget(parent)
    , m_gridSettingsWidget(new GridSettingsWidget(this))
    , m_legendTool(new QuickOverlayLegend(this))
    , m_inspectorInterface(inspector)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(QMargins());

    m_previewWidget = new QuickScenePreviewWidget(m_inspectorInterface, this, this);

    m_toolBar = new QToolBar(this);
    m_toolBar->setAutoFillBackground(true);

    m_visualizeGroup = new QActionGroup(this);
    m_visualizeGroup->setExclusive(false); // we need 0 or 1 selected, not exactly 1

    m_visualizeClipping = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/visualize-clipping.png")),
                                      tr("Visualize Clipping"),
                                      this);
    m_visualizeClipping->setActionGroup(m_visualizeGroup);
    m_visualizeClipping->setData(QuickInspectorInterface::VisualizeClipping);
    m_visualizeClipping->setCheckable(true);
    m_visualizeClipping->setToolTip(tr("<b>Visualize Clipping</b><br/>"
                                       "Items with the property <i>clip</i> set to true, will cut off their and their "
                                       "children's rendering at the items' bounds. While this is a handy feature it "
                                       "comes with quite some cost, like disabling some performance optimizations.<br/>"
                                       "With this tool enabled the QtQuick renderer highlights items, that have clipping "
                                       "enabled, so you can check for items, that have clipping enabled unnecessarily. "));

    m_visualizeOverdraw
            = new QAction(QIcon(QStringLiteral(
                                    ":/gammaray/plugins/quickinspector/visualize-overdraw.png")),
                          tr("Visualize Overdraw"),
                          this);
    m_visualizeOverdraw->setActionGroup(m_visualizeGroup);
    m_visualizeOverdraw->setData(QuickInspectorInterface::VisualizeOverdraw);
    m_visualizeOverdraw->setCheckable(true);
    m_visualizeOverdraw->setToolTip(tr("<b>Visualize Overdraw</b><br/>"
                                       "The QtQuick renderer doesn't detect if an item is obscured by another "
                                       "opaque item, is completely outside the scene or outside a clipped ancestor and "
                                       "thus doesn't need to be rendered. You thus need to take care of setting "
                                       "<i>visible: false</i> for hidden items, yourself.<br/>"
                                       "With this tool enabled the QtQuick renderer draws a 3D-Box visualizing the "
                                       "layers of items that are drawn."));

    m_visualizeBatches = new QAction(QIcon(QStringLiteral(
                                               ":/gammaray/plugins/quickinspector/visualize-batches.png")),
                                     tr("Visualize Batches"), this);
    m_visualizeBatches->setActionGroup(m_visualizeGroup);
    m_visualizeBatches->setData(QuickInspectorInterface::VisualizeBatches);
    m_visualizeBatches->setCheckable(true);
    m_visualizeBatches->setToolTip(tr("<b>Visualize Batches</b><br/>"
                                      "Where a traditional 2D API, such as QPainter, Cairo or Context2D, is written to "
                                      "handle thousands of individual draw calls per frame, OpenGL is a pure hardware "
                                      "API and performs best when the number of draw calls is very low and state "
                                      "changes are kept to a minimum. Therefore the QtQuick renderer combines the "
                                      "rendering of similar items into single batches.<br/>"
                                      "Some settings (like <i>clip: true</i>) will cause the batching to fail, though, "
                                      "causing items to be rendered separately. With this tool enabled the QtQuick "
                                      "renderer visualizes those batches, by drawing all items that are batched using "
                                      "the same color. The fewer colors you see in this mode the better."));

    m_visualizeChanges = new QAction(QIcon(QStringLiteral(
                                               ":/gammaray/plugins/quickinspector/visualize-changes.png")),
                                     tr("Visualize Changes"), this);
    m_visualizeChanges->setActionGroup(m_visualizeGroup);
    m_visualizeChanges->setData(QuickInspectorInterface::VisualizeChanges);
    m_visualizeChanges->setCheckable(true);
    m_visualizeChanges->setToolTip(tr("<b>Visualize Changes</b><br>"
                                      "The QtQuick scene is only repainted, if some item changes in a visual manner. "
                                      "Unnecessary repaints can have a bad impact on the performance. With this tool "
                                      "enabled, the QtQuick renderer will thus on each repaint highlight the item(s), "
                                      "that caused the repaint."));

    m_visualizeTraces
            = new QAction(QIcon(QStringLiteral(
                                    ":/gammaray/plugins/quickinspector/visualize-traces.png")),
                          tr("Visualize Traces"), this);
    m_visualizeTraces->setActionGroup(m_visualizeGroup);
    m_visualizeTraces->setData(QuickInspectorInterface::VisualizeTraces);
    m_visualizeTraces->setCheckable(true);
    m_visualizeTraces->setToolTip(tr("<b>Visualize Traces</b><br>"
                                     "The QtQuick scene is rendered normaly, in addition overlay rects will "
                                     "cover any QQ2 components. Overlay include random border and foreground "
                                     "colors as well as item id string."));

    m_serverSideDecorationsEnabled = new QAction(QIcon(QStringLiteral(
                                                           ":/gammaray/plugins/quickinspector/decorations.png")),
                                                 tr("Target Decorations"), this);
    m_serverSideDecorationsEnabled->setCheckable(true);
    m_serverSideDecorationsEnabled->setToolTip(tr("<b>Target Decorations</b><br>"
                                                  "This enable or not the decorations on the target application."));

    QWidgetAction *gridSettingsAction = new QWidgetAction(this);
    gridSettingsAction->setDefaultWidget(m_gridSettingsWidget);

    m_gridSettings = new QMenu(tr("Grid Settings"), this);
    m_gridSettings->setIcon(QIcon(QStringLiteral(
                                      ":/gammaray/plugins/quickinspector/grid-settings.png")));
    m_gridSettings->setToolTip(tr("<b>Grid Settings</b><br>"
                                  "This popup a small widget to configure the grid settings."));
    m_gridSettings->setToolTipsVisible(true);
    m_gridSettings->addAction(gridSettingsAction);

    m_toolBar->addActions(m_visualizeGroup->actions());
    connect(m_visualizeGroup, SIGNAL(triggered(QAction*)), this,
            SLOT(visualizeActionTriggered(QAction*)));

    m_toolBar->addSeparator();

    foreach (auto action, m_previewWidget->interactionModeActions()->actions()) {
        m_toolBar->addAction(action);
    }
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_serverSideDecorationsEnabled);
    connect(m_serverSideDecorationsEnabled, SIGNAL(triggered(bool)), this,
            SLOT(serverSideDecorationsTriggered(bool)));
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_previewWidget->zoomOutAction());
    m_zoomCombobox = new QComboBox(this);
    m_zoomCombobox->setModel(m_previewWidget->zoomLevelModel());
    connect(m_zoomCombobox, SIGNAL(currentIndexChanged(int)), m_previewWidget,
            SLOT(setZoomLevel(int)));
    connect(m_previewWidget, &RemoteViewWidget::zoomLevelChanged, m_zoomCombobox,
            &QComboBox::setCurrentIndex);
    m_zoomCombobox->setCurrentIndex(m_previewWidget->zoomLevelIndex());

    m_toolBar->addWidget(m_zoomCombobox);
    m_toolBar->addAction(m_previewWidget->zoomInAction());

    m_toolBar->addSeparator();
    m_toolBar->addAction(m_legendTool->visibilityAction());
    m_toolBar->addAction(m_gridSettings->menuAction());

    QToolButton *b = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_gridSettings->menuAction()));
    b->setPopupMode(QToolButton::InstantPopup);

    connect(m_gridSettingsWidget, SIGNAL(offsetChanged(QPoint)), this, SLOT(gridOffsetChanged(QPoint)));
    connect(m_gridSettingsWidget, SIGNAL(cellSizeChanged(QSize)), this, SLOT(gridCellSizeChanged(QSize)));

    setMinimumWidth(std::max(minimumWidth(), m_toolBar->sizeHint().width()));

    m_layout->setMenuBar(m_toolBar);
    m_layout->addWidget(m_previewWidget);

    connect(m_previewWidget, SIGNAL(stateChanged()), this, SIGNAL(stateChanged()));
}

void QuickSceneControlWidget::resizeEvent(QResizeEvent *e)
{
    m_toolBar->setGeometry(0, 0, width(), m_toolBar->sizeHint().height());
    QWidget::resizeEvent(e);
}

void QuickSceneControlWidget::visualizeActionTriggered(QAction *current)
{
    if (!current || !current->isChecked()) {
        m_inspectorInterface->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
    } else {
        // QActionGroup requires exactly one selected, but we need 0 or 1 selected
        foreach (auto action, m_visualizeGroup->actions()) {
            if (action != current)
                action->setChecked(false);
        }

        m_inspectorInterface->setCustomRenderMode(static_cast<QuickInspectorInterface::RenderMode>(current->data().toInt()));
    }
    emit m_previewWidget->stateChanged();
}

void QuickSceneControlWidget::serverSideDecorationsTriggered(bool enabled)
{
    m_serverSideDecorationsEnabled->setChecked(enabled);
    m_inspectorInterface->setServerSideDecorationsEnabled(enabled);
    emit m_previewWidget->stateChanged();
}

void QuickSceneControlWidget::gridOffsetChanged(const QPoint &value)
{
    m_previewWidget->m_overlaySettings.gridOffset = value;
    m_legendTool->setOverlaySettings(m_previewWidget->m_overlaySettings);
    update();
    setOverlaySettings(m_previewWidget->m_overlaySettings);
}

void QuickSceneControlWidget::gridCellSizeChanged(const QSize &value)
{
    m_previewWidget->m_overlaySettings.gridCellSize = value;
    m_legendTool->setOverlaySettings(m_previewWidget->m_overlaySettings);
    update();
    setOverlaySettings(m_previewWidget->m_overlaySettings);
}

void QuickSceneControlWidget::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    m_inspectorInterface->setOverlaySettings(settings);
    emit m_previewWidget->stateChanged();
}

void QuickSceneControlWidget::setSupportsCustomRenderModes(
        QuickInspectorInterface::Features supportedCustomRenderModes)
{
    m_visualizeClipping->setEnabled(
                supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeClipping);
    m_visualizeBatches->setEnabled(
                supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeBatches);
    m_visualizeOverdraw->setEnabled(
                supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeOverdraw);
    m_visualizeChanges->setEnabled(
                supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeChanges);
    m_visualizeTraces->setEnabled(
                supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeTraces);
}

void QuickSceneControlWidget::setServerSideDecorationsState(bool enabled)
{
    m_serverSideDecorationsEnabled->setChecked(enabled);
}

void QuickSceneControlWidget::setOverlaySettingsState(const QuickDecorationsSettings &settings)
{
    m_previewWidget->m_overlaySettings = settings;
    m_gridSettingsWidget->setOverlaySettings(settings);
    m_legendTool->setOverlaySettings(settings);
}

QuickInspectorInterface::RenderMode QuickSceneControlWidget::customRenderMode() const
{
    const QAction *checkedAction = m_visualizeGroup->checkedAction();

    if (checkedAction) {
        return static_cast<QuickInspectorInterface::RenderMode>(checkedAction->data().toInt());
    }

    return QuickInspectorInterface::NormalRendering;
}

void QuickSceneControlWidget::setCustomRenderMode(QuickInspectorInterface::RenderMode customRenderMode)
{
    if (this->customRenderMode() == customRenderMode)
        return;

    foreach (auto action, m_visualizeGroup->actions()) {
        if (action)
            action->setChecked(static_cast<QuickInspectorInterface::RenderMode>(action->data().toInt()) == customRenderMode);
    }

    visualizeActionTriggered(m_visualizeGroup->checkedAction());
}

QuickScenePreviewWidget *QuickSceneControlWidget::previewWidget()
{
    return m_previewWidget;
}

bool QuickSceneControlWidget::serverSideDecorationsEnabled() const
{
    return m_serverSideDecorationsEnabled->isChecked();
}

void QuickSceneControlWidget::setServerSideDecorationsEnabled(bool enabled)
{
    if (m_serverSideDecorationsEnabled->isChecked() == enabled)
        return;
    m_serverSideDecorationsEnabled->setChecked(enabled);
    serverSideDecorationsTriggered(enabled);
}
