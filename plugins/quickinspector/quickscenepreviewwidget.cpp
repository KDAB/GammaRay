/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "quickscenepreviewwidget.h"
#include "quickinspectorinterface.h"
#include "quickoverlaylegend.h"
#include "gridsettingswidget.h"

#include <common/streamoperators.h>

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QWidgetAction>
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>

#include <cmath>

using namespace GammaRay;
static qint32 QuickScenePreviewWidgetStateVersion = 2;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::QuickInspectorInterface::RenderMode)
QT_END_NAMESPACE

QuickScenePreviewWidget::QuickScenePreviewWidget(QuickInspectorInterface *inspector,
                                                 QWidget *parent)
    : RemoteViewWidget(parent)
    , m_gridSettingsWidget(new GridSettingsWidget(this))
    , m_legendTool(new QuickOverlayLegend(this))
    , m_inspectorInterface(inspector)
{
    setName(QStringLiteral("com.kdab.GammaRay.QuickRemoteView"));

    // Toolbar
    m_toolBar.toolbarWidget = new QToolBar(this);
    m_toolBar.toolbarWidget->setAutoFillBackground(true);

    m_toolBar.visualizeGroup = new QActionGroup(this);
    m_toolBar.visualizeGroup->setExclusive(false); // we need 0 or 1 selected, not exactly 1

    m_toolBar.visualizeClipping
        = new QAction(QIcon(QStringLiteral(
                                ":/gammaray/plugins/quickinspector/visualize-clipping.png")),
                      tr("Visualize Clipping"),
                      this);
    m_toolBar.visualizeClipping->setActionGroup(m_toolBar.visualizeGroup);
    m_toolBar.visualizeClipping->setCheckable(true);
    m_toolBar.visualizeClipping->setToolTip(tr("<b>Visualize Clipping</b><br/>"
                                               "Items with the property <i>clip</i> set to true, will cut off their and their "
                                               "children's rendering at the items' bounds. While this is a handy feature it "
                                               "comes with quite some cost, like disabling some performance optimizations.<br/>"
                                               "With this tool enabled the QtQuick renderer highlights items, that have clipping "
                                               "enabled, so you can check for items, that have clipping enabled unnecessarily. "));

    m_toolBar.visualizeOverdraw
        = new QAction(QIcon(QStringLiteral(
                                ":/gammaray/plugins/quickinspector/visualize-overdraw.png")),
                      tr("Visualize Overdraw"),
                      this);
    m_toolBar.visualizeOverdraw->setActionGroup(m_toolBar.visualizeGroup);
    m_toolBar.visualizeOverdraw->setCheckable(true);
    m_toolBar.visualizeOverdraw->setToolTip(tr("<b>Visualize Overdraw</b><br/>"
                                               "The QtQuick renderer doesn't detect if an item is obscured by another "
                                               "opaque item, is completely outside the scene or outside a clipped ancestor and "
                                               "thus doesn't need to be rendered. You thus need to take care of setting "
                                               "<i>visible: false</i> for hidden items, yourself.<br/>"
                                               "With this tool enabled the QtQuick renderer draws a 3D-Box visualizing the "
                                               "layers of items that are drawn."));

    m_toolBar.visualizeBatches
        = new QAction(QIcon(QStringLiteral(
                                ":/gammaray/plugins/quickinspector/visualize-batches.png")),
                      tr("Visualize Batches"), this);
    m_toolBar.visualizeBatches->setActionGroup(m_toolBar.visualizeGroup);
    m_toolBar.visualizeBatches->setCheckable(true);
    m_toolBar.visualizeBatches->setToolTip(tr("<b>Visualize Batches</b><br/>"
                                              "Where a traditional 2D API, such as QPainter, Cairo or Context2D, is written to "
                                              "handle thousands of individual draw calls per frame, OpenGL is a pure hardware "
                                              "API and performs best when the number of draw calls is very low and state "
                                              "changes are kept to a minimum. Therefore the QtQuick renderer combines the "
                                              "rendering of similar items into single batches.<br/>"
                                              "Some settings (like <i>clip: true</i>) will cause the batching to fail, though, "
                                              "causing items to be rendered separately. With this tool enabled the QtQuick "
                                              "renderer visualizes those batches, by drawing all items that are batched using "
                                              "the same color. The fewer colors you see in this mode the better."));

    m_toolBar.visualizeChanges
        = new QAction(QIcon(QStringLiteral(
                                ":/gammaray/plugins/quickinspector/visualize-changes.png")),
                      tr("Visualize Changes"), this);
    m_toolBar.visualizeChanges->setActionGroup(m_toolBar.visualizeGroup);
    m_toolBar.visualizeChanges->setCheckable(true);
    m_toolBar.visualizeChanges->setToolTip(tr("<b>Visualize Changes</b><br>"
                                              "The QtQuick scene is only repainted, if some item changes in a visual manner. "
                                              "Unnecessary repaints can have a bad impact on the performance. With this tool "
                                              "enabled, the QtQuick renderer will thus on each repaint highlight the item(s), "
                                              "that caused the repaint."));

    m_toolBar.serverSideDecorationsEnabled = new QAction(QIcon(QStringLiteral(
                                                               ":/gammaray/plugins/quickinspector/active-focus.png")),
                                                     tr("Target Decorations"), this);
    m_toolBar.serverSideDecorationsEnabled->setCheckable(true);
    m_toolBar.serverSideDecorationsEnabled->setToolTip(tr("<b>Target Decorations</b><br>"
                                              "This enable or not the decorations on the target application."));

    QWidgetAction *gridSettingsAction = new QWidgetAction(this);
    gridSettingsAction->setDefaultWidget(m_gridSettingsWidget);

    m_toolBar.gridSettings = new QMenu(tr("Grid Settings"), this);
    m_toolBar.gridSettings->setIcon(QIcon(QStringLiteral(
                                              ":/gammaray/plugins/quickinspector/active-focus.png")));
    m_toolBar.gridSettings->setToolTip(tr("<b>Grid Settings</b><br>"
                                              "This popup a small widget to configure the grid settings."));
    m_toolBar.gridSettings->setToolTipsVisible(true);
    m_toolBar.gridSettings->addAction(gridSettingsAction);

    m_toolBar.toolbarWidget->addActions(m_toolBar.visualizeGroup->actions());
    connect(m_toolBar.visualizeGroup, SIGNAL(triggered(QAction*)), this,
            SLOT(visualizeActionTriggered(QAction*)));

    m_toolBar.toolbarWidget->addSeparator();

    foreach (auto action, interactionModeActions()->actions()) {
        m_toolBar.toolbarWidget->addAction(action);
    }
    m_toolBar.toolbarWidget->addSeparator();

    m_toolBar.toolbarWidget->addAction(m_toolBar.serverSideDecorationsEnabled);
    connect(m_toolBar.serverSideDecorationsEnabled, SIGNAL(triggered(bool)), this,
            SLOT(serverSideDecorationsTriggered(bool)));
    m_toolBar.toolbarWidget->addSeparator();

    m_toolBar.toolbarWidget->addAction(zoomOutAction());
    m_toolBar.zoomCombobox = new QComboBox(this);
    m_toolBar.zoomCombobox->setModel(zoomLevelModel());
    connect(m_toolBar.zoomCombobox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setZoomLevel(int)));
    connect(this, &RemoteViewWidget::zoomLevelChanged, m_toolBar.zoomCombobox,
            &QComboBox::setCurrentIndex);
    m_toolBar.zoomCombobox->setCurrentIndex(zoomLevelIndex());

    m_toolBar.toolbarWidget->addWidget(m_toolBar.zoomCombobox);
    m_toolBar.toolbarWidget->addAction(zoomInAction());

    m_toolBar.toolbarWidget->addSeparator();
    m_toolBar.toolbarWidget->addAction(m_legendTool->visibilityAction());
    m_toolBar.toolbarWidget->addAction(m_toolBar.gridSettings->menuAction());

    QToolButton *b = qobject_cast<QToolButton *>(m_toolBar.toolbarWidget->widgetForAction(m_toolBar.gridSettings->menuAction()));
    b->setPopupMode(QToolButton::InstantPopup);

    connect(m_gridSettingsWidget, SIGNAL(offsetChanged(QPoint)), this, SLOT(gridOffsetChanged(QPoint)));
    connect(m_gridSettingsWidget, SIGNAL(cellSizeChanged(QSize)), this, SLOT(gridCellSizeChanged(QSize)));
    connect(this, SIGNAL(interactionModeChanged()), this, SLOT(interactionChanged()));

    setUnavailableText(tr(
                           "No remote view available.\n(This happens e.g. when the window is minimized or the scene is hidden)"));

    setMinimumWidth(std::max(minimumWidth(), m_toolBar.toolbarWidget->sizeHint().width()));
}

QuickScenePreviewWidget::~QuickScenePreviewWidget()
{
}

void QuickScenePreviewWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return;

    QDataStream stream(state);
    qint32 version;
    QuickInspectorInterface::RenderMode mode = customRenderMode();
    bool drawDecorations = serverSideDecorationsEnabled();
    RemoteViewWidget::restoreState(stream);

    stream >> version;

    switch (version) {
    case 1: {
        stream
                >> mode
        ;
        break;
    }
    case 2: {
        stream
                >> mode
                >> drawDecorations
        ;
        break;
    }
    }

    setCustomRenderMode(mode);
    setServerSideDecorationsEnabled(drawDecorations);
}

QByteArray QuickScenePreviewWidget::saveState() const
{
    QByteArray data;

    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        RemoteViewWidget::saveState(stream);

        stream << QuickScenePreviewWidgetStateVersion;

        switch (QuickScenePreviewWidgetStateVersion) {
        case 1: {
            stream
                    << customRenderMode()
            ;
            break;
        }
        case 2: {
            stream
                    << customRenderMode()
                    << serverSideDecorationsEnabled()
            ;
            break;
        }
        }
    }

    return data;
}

void QuickScenePreviewWidget::resizeEvent(QResizeEvent *e)
{
    m_toolBar.toolbarWidget->setGeometry(0, 0, width(),
                                         m_toolBar.toolbarWidget->sizeHint().height());
    RemoteViewWidget::resizeEvent(e);
}

void QuickScenePreviewWidget::drawDecoration(QPainter *p)
{
    if (frame().data().userType() == qMetaTypeId<QuickItemGeometry>()) {
        // scaled and translated
        auto itemGeometry = frame().data().value<QuickItemGeometry>();
        itemGeometry.scaleTo(zoom());
        const QuickDecorationsRenderInfo renderInfo(m_overlaySettings, itemGeometry, frame().viewRect(), zoom());
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Decorations, *p, renderInfo);
        drawer.drawDecorations();
    } else if (frame().data().userType() == qMetaTypeId<QVector<QuickItemGeometry>>()) {
        // Scaling and translations will be done on demand
        const auto itemsGeometry = frame().data().value<QVector<QuickItemGeometry>>();
        const QuickDecorationsTracesInfo tracesInfo(m_overlaySettings, itemsGeometry, frame().viewRect(), zoom());
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Traces, *p, tracesInfo);
        drawer.drawTraces();
    }
}

void QuickScenePreviewWidget::visualizeActionTriggered(QAction *current)
{
    if (!current->isChecked()) {
        m_inspectorInterface->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
    } else {
        // QActionGroup requires exactly one selected, but we need 0 or 1 selected
        foreach (auto action, m_toolBar.visualizeGroup->actions()) {
            if (action != current)
                action->setChecked(false);
        }
        m_inspectorInterface->setCustomRenderMode(current == m_toolBar.visualizeClipping ? QuickInspectorInterface::VisualizeClipping
                                                  : current == m_toolBar.visualizeBatches ? QuickInspectorInterface::VisualizeBatches
                                                  : current == m_toolBar.visualizeOverdraw ? QuickInspectorInterface::VisualizeOverdraw
                                                  : current == m_toolBar.visualizeChanges ? QuickInspectorInterface::VisualizeChanges
                                                  : QuickInspectorInterface::NormalRendering
                                                  );
    }
    emit stateChanged();
}

void QuickScenePreviewWidget::serverSideDecorationsTriggered(bool enabled)
{
    m_toolBar.serverSideDecorationsEnabled->setChecked(enabled);
    m_inspectorInterface->setServerSideDecorationsEnabled(enabled);
    emit stateChanged();
}

void QuickScenePreviewWidget::gridOffsetChanged(const QPoint &value)
{
    m_overlaySettings.gridOffset = value;
    m_legendTool->setOverlaySettings(m_overlaySettings);
    update();
    setOverlaySettings(m_overlaySettings);
}

void QuickScenePreviewWidget::gridCellSizeChanged(const QSize &value)
{
    m_overlaySettings.gridCellSize = value;
    m_legendTool->setOverlaySettings(m_overlaySettings);
    update();
    setOverlaySettings(m_overlaySettings);
}

void QuickScenePreviewWidget::interactionChanged()
{
    const bool tracing = (interactionMode() == ComponentTraces);
    if (m_overlaySettings.componentsTraces != tracing) {
        m_overlaySettings.componentsTraces = tracing;
        setOverlaySettings(m_overlaySettings);
    }
}

void GammaRay::QuickScenePreviewWidget::setSupportsCustomRenderModes(
    QuickInspectorInterface::Features supportedCustomRenderModes)
{
    m_toolBar.visualizeClipping->setEnabled(
        supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeClipping);
    m_toolBar.visualizeBatches->setEnabled(
        supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeBatches);
    m_toolBar.visualizeOverdraw->setEnabled(
        supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeOverdraw);
    m_toolBar.visualizeChanges->setEnabled(
        supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeChanges);
}

void QuickScenePreviewWidget::setServerSideDecorationsState(bool enabled)
{
    m_toolBar.serverSideDecorationsEnabled->setChecked(enabled);
}

void QuickScenePreviewWidget::setOverlaySettingsState(const QuickDecorationsSettings &settings)
{
    m_overlaySettings = settings;
    m_gridSettingsWidget->setOverlaySettings(settings);
    m_legendTool->setOverlaySettings(settings);
}

QuickInspectorInterface::RenderMode QuickScenePreviewWidget::customRenderMode() const
{
    if (m_toolBar.visualizeClipping->isChecked()) {
        return QuickInspectorInterface::VisualizeClipping;
    }
    else if (m_toolBar.visualizeBatches->isChecked()) {
        return QuickInspectorInterface::VisualizeBatches;
    }
    else if (m_toolBar.visualizeOverdraw->isChecked()) {
        return QuickInspectorInterface::VisualizeOverdraw;
    }
    else if (m_toolBar.visualizeChanges->isChecked()) {
        return QuickInspectorInterface::VisualizeChanges;
    }

    return QuickInspectorInterface::NormalRendering;
}

void QuickScenePreviewWidget::setCustomRenderMode(QuickInspectorInterface::RenderMode customRenderMode)
{
    if (this->customRenderMode() == customRenderMode)
        return;

    QAction *currentAction = nullptr;
    switch (customRenderMode) {
    case QuickInspectorInterface::NormalRendering:
        break;
    case QuickInspectorInterface::VisualizeClipping:
        currentAction = m_toolBar.visualizeClipping;
        break;
    case QuickInspectorInterface::VisualizeOverdraw:
        currentAction = m_toolBar.visualizeOverdraw;
        break;
    case QuickInspectorInterface::VisualizeBatches:
        currentAction = m_toolBar.visualizeBatches;
        break;
    case QuickInspectorInterface::VisualizeChanges:
        currentAction = m_toolBar.visualizeChanges;
        break;
    }

    foreach (auto action, m_toolBar.visualizeGroup->actions()) {
        if (action)
            action->setChecked(currentAction == action);
    }

    visualizeActionTriggered(currentAction ? currentAction : m_toolBar.visualizeBatches);
}

QuickDecorationsSettings QuickScenePreviewWidget::overlaySettings() const
{
    return m_overlaySettings;
}

void QuickScenePreviewWidget::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    m_inspectorInterface->setOverlaySettings(settings);
    emit stateChanged();
}

bool QuickScenePreviewWidget::serverSideDecorationsEnabled() const
{
    return m_toolBar.serverSideDecorationsEnabled->isChecked();
}

void QuickScenePreviewWidget::setServerSideDecorationsEnabled(bool enabled)
{
    if (m_toolBar.serverSideDecorationsEnabled->isChecked() == enabled)
        return;
    m_toolBar.serverSideDecorationsEnabled->setChecked(enabled);
    serverSideDecorationsTriggered(enabled);
}
