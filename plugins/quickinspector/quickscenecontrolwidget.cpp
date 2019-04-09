/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <ui/uiresources.h>

#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

using namespace GammaRay;

namespace {
static QAction *checkedAction(QActionGroup *group) {
    // For some reason QActionGroup::checkedAction() is not yet synced when needed
    // Might be because of our direct QAction::setChecked() calls in between
    foreach (QAction *action, group->actions()) {
        if (action->isChecked())
            return action;
    }

    return nullptr;
}
}

QuickSceneControlWidget::QuickSceneControlWidget(QuickInspectorInterface *inspector, QWidget *parent)
    : QWidget(parent)
    , m_gridSettingsWidget(new GridSettingsWidget) // Owned by the QWidgetAction
    , m_legendTool(new QuickOverlayLegend(this))
    , m_inspectorInterface(inspector)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(QMargins());

    m_previewWidget = new QuickScenePreviewWidget(this, this);

    m_toolBar = new QToolBar(this);
    m_toolBar->setAutoFillBackground(true);
    // Our icons are 16x16 and support hidpi, so let force iconSize on every styles
    m_toolBar->setIconSize(QSize(16, 16));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    m_visualizeGroup = new QActionGroup(this);
    m_visualizeGroup->setExclusive(false); // we need 0 or 1 selected, not exactly 1

    m_visualizeClipping = new QAction(UIResources::themedIcon(QLatin1String("visualize-clipping.png")),
                                      tr("Visualize Clipping"),
                                      this);
    m_visualizeClipping->setObjectName("aVisualizeClipping");
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
            = new QAction(UIResources::themedIcon(QLatin1String("visualize-overdraw.png")),
                          tr("Visualize Overdraw"),
                          this);
    m_visualizeOverdraw->setObjectName("aVisualizeOverdraw");
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

    m_visualizeBatches = new QAction(UIResources::themedIcon(QLatin1String("visualize-batches.png")),
                                     tr("Visualize Batches"), this);
    m_visualizeBatches->setObjectName("aVisualizeBatches");
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

    m_visualizeChanges = new QAction(UIResources::themedIcon(QLatin1String("visualize-changes.png")),
                                     tr("Visualize Changes"), this);
    m_visualizeChanges->setObjectName("aVisualizeChanges");
    m_visualizeChanges->setActionGroup(m_visualizeGroup);
    m_visualizeChanges->setData(QuickInspectorInterface::VisualizeChanges);
    m_visualizeChanges->setCheckable(true);
    m_visualizeChanges->setToolTip(tr("<b>Visualize Changes</b><br>"
                                      "The QtQuick scene is only repainted, if some item changes in a visual manner. "
                                      "Unnecessary repaints can have a bad impact on the performance. With this tool "
                                      "enabled, the QtQuick renderer will thus on each repaint highlight the item(s), "
                                      "that caused the repaint."));

    m_visualizeTraces
            = new QAction(UIResources::themedIcon(QLatin1String("visualize-traces.png")),
                          tr("Visualize Controls"), this);
    m_visualizeTraces->setObjectName("aVisualizeTraces");
    m_visualizeTraces->setActionGroup(m_visualizeGroup);
    m_visualizeTraces->setData(QuickInspectorInterface::VisualizeTraces);
    m_visualizeTraces->setCheckable(true);
    m_visualizeTraces->setToolTip(tr("<b>Visualize Controls</b><br>"
                                     "The QtQuick scene is rendered normaly, in addition overlays will "
                                     "highlight Qt Quick Controls."));

    m_serverSideDecorationsEnabled = new QAction(UIResources::themedIcon(QLatin1String("server-decorations.png")),
                                                 tr("Decorate Target"), this);
    m_serverSideDecorationsEnabled->setObjectName("aServerSideDecorationsEnabled");
    m_serverSideDecorationsEnabled->setCheckable(true);
    m_serverSideDecorationsEnabled->setToolTip(tr("<b>Decorate Target</b><br>"
                                                  "This enables diagnostic decorations on the target application, "
                                                  "such as anchor and layout information, helper grids or control overlays."));

    auto *gridSettingsAction = new QWidgetAction(this);
    gridSettingsAction->setObjectName("aGridSettingsAction");
    gridSettingsAction->setDefaultWidget(m_gridSettingsWidget);

    m_gridSettingsMenu = new QMenu(tr("Layout Grid"), this);
    m_gridSettingsMenu->setObjectName("mLayoutGrid");
    m_gridSettingsMenu->menuAction()->setObjectName("aLayoutGrid");
    m_gridSettingsMenu->setIcon(UIResources::themedIcon(QLatin1String("grid-settings.png")));
    m_gridSettingsMenu->setToolTip(tr("<b>Layout Grid</b><br>"
                                  "Configure the layout helper grid."));
    m_gridSettingsMenu->setToolTipsVisible(true);
    m_gridSettingsMenu->addAction(gridSettingsAction);

#if defined(Q_OS_MAC)
    // Workaround QTBUG-60424
    connect(m_gridSettingsMenu, &QMenu::aboutToShow,
            m_gridSettingsWidget, static_cast<void (QWidget::*)()>(&QWidget::update));
#endif

    m_toolBar->addActions(m_visualizeGroup->actions());
    connect(m_visualizeGroup, &QActionGroup::triggered, this,
            &QuickSceneControlWidget::visualizeActionTriggered);

    m_toolBar->addSeparator();

    m_toolBar->addActions(m_previewWidget->interactionModeActions()->actions());

    m_toolBar->addSeparator();

    m_toolBar->addAction(m_serverSideDecorationsEnabled);
    connect(m_serverSideDecorationsEnabled, &QAction::triggered, this,
            &QuickSceneControlWidget::serverSideDecorationsTriggered);
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_previewWidget->zoomOutAction());
    m_zoomCombobox = new QComboBox(this);
    m_zoomCombobox->setModel(m_previewWidget->zoomLevelModel());
    // macOS and some platforms expect to use *small* controls in such small toolbar
    m_zoomCombobox->setAttribute(Qt::WA_MacSmallSize);
    connect(m_zoomCombobox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            m_previewWidget, &RemoteViewWidget::setZoomLevel);
    connect(m_previewWidget, &RemoteViewWidget::zoomLevelChanged, m_zoomCombobox,
            &QComboBox::setCurrentIndex);
    m_zoomCombobox->setCurrentIndex(m_previewWidget->zoomLevelIndex());

    QAction *zoomComboBox = m_toolBar->addWidget(m_zoomCombobox);
    zoomComboBox->setObjectName("aZoomComboBox");
    m_toolBar->addAction(m_previewWidget->zoomInAction());

    connect(m_gridSettingsWidget, &GridSettingsWidget::enabledChanged, this, &QuickSceneControlWidget::gridEnabledChanged);
    connect(m_gridSettingsWidget, &GridSettingsWidget::offsetChanged, this, &QuickSceneControlWidget::gridOffsetChanged);
    connect(m_gridSettingsWidget, &GridSettingsWidget::cellSizeChanged, this, &QuickSceneControlWidget::gridCellSizeChanged);

    setMinimumWidth(std::max(minimumWidth(), m_toolBar->sizeHint().width()));

    m_layout->setMenuBar(m_toolBar);
    m_layout->addWidget(m_previewWidget);

    connect(m_previewWidget, &RemoteViewWidget::stateChanged, this, &QuickSceneControlWidget::stateChanged);

    auto *menuSeparator = new QAction(this);
    menuSeparator->setSeparator(true);

    addActions(m_toolBar->actions()
               << menuSeparator
               << m_legendTool->visibilityAction()
               << m_gridSettingsMenu->menuAction());
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

void QuickSceneControlWidget::gridEnabledChanged(bool enabled)
{
    QuickDecorationsSettings settings = m_previewWidget->overlaySettings();
    settings.gridEnabled = enabled;
    setOverlaySettings(settings);
}

void QuickSceneControlWidget::gridOffsetChanged(const QPoint &value)
{
    QuickDecorationsSettings settings = m_previewWidget->overlaySettings();
    settings.gridOffset = value;
    setOverlaySettings(settings);
}

void QuickSceneControlWidget::gridCellSizeChanged(const QSize &value)
{
    QuickDecorationsSettings settings = m_previewWidget->overlaySettings();
    settings.gridCellSize = value;
    setOverlaySettings(settings);
}

void QuickSceneControlWidget::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    m_inspectorInterface->setOverlaySettings(settings);
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
    m_previewWidget->setOverlaySettings(settings);
    m_gridSettingsWidget->setOverlaySettings(settings);
    m_legendTool->setOverlaySettings(settings);
    emit m_previewWidget->stateChanged();
}

QuickInspectorInterface::RenderMode QuickSceneControlWidget::customRenderMode() const
{
    const QAction *checkedAction = ::checkedAction(m_visualizeGroup);

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

    visualizeActionTriggered(checkedAction(m_visualizeGroup));
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
