/*
  quickscenepreviewwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef QUICKSCENECONTROLWIDGET_H
#define QUICKSCENECONTROLWIDGET_H

#include <QWidget>

#include "quickdecorationsdrawer.h"
#include "quickinspectorinterface.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QComboBox;
class QLabel;
class QMenu;
class QToolBar;
class QVBoxLayout;
QT_END_NAMESPACE

namespace GammaRay {
class QuickInspectorInterface;
class GridSettingsWidget;
class QuickScenePreviewWidget;
class QuickOverlayLegend;

class QuickSceneControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickSceneControlWidget(QuickInspectorInterface *inspector, QWidget *parent = nullptr);

    Q_INVOKABLE void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings);
    void setSupportsCustomRenderModes(QuickInspectorInterface::Features supportedCustomRenderModes);
    void setServerSideDecorationsState(bool enabled);

    void setOverlaySettingsState(const QuickDecorationsSettings &settings);

    bool serverSideDecorationsEnabled() const;
    void setServerSideDecorationsEnabled(bool enabled);

    QuickInspectorInterface::RenderMode customRenderMode() const;
    void setCustomRenderMode(QuickInspectorInterface::RenderMode customRenderMode);

    QuickScenePreviewWidget *previewWidget();

signals:
    void stateChanged();

private Q_SLOTS:
    void visualizeActionTriggered(QAction* current);
    void serverSideDecorationsTriggered(bool enabled);
    void gridEnabledChanged(bool enabled);
    void gridOffsetChanged(const QPoint &value);
    void gridCellSizeChanged(const QSize &value);

private:
    void resizeEvent(QResizeEvent *e) override;

    QVBoxLayout *m_layout;

    QToolBar *m_toolBar;
    QComboBox *m_zoomCombobox;
    QActionGroup *m_visualizeGroup;
    QAction *m_visualizeClipping;
    QAction *m_visualizeOverdraw;
    QAction *m_visualizeBatches;
    QAction *m_visualizeChanges;
    QAction *m_visualizeTraces;
    QAction *m_serverSideDecorationsEnabled;
    QMenu *m_gridSettingsMenu;

    QuickScenePreviewWidget *m_previewWidget;

    GridSettingsWidget *m_gridSettingsWidget;
    QuickOverlayLegend *m_legendTool;

    QuickInspectorInterface *m_inspectorInterface;
};
} // namespace GammaRay

#endif // QUICKSCENECONTROLWIDGET_H
