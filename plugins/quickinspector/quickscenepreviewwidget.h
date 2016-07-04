/*
  quickscenepreviewwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H

#include "quickitemgeometry.h"
#include "quickinspectorinterface.h"

#include <ui/remoteviewwidget.h>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QComboBox;
class QLabel;
class QToolBar;
QT_END_NAMESPACE

namespace GammaRay {
class QuickInspectorInterface;

class QuickScenePreviewWidget : public RemoteViewWidget
{
    Q_OBJECT

public:
    explicit QuickScenePreviewWidget(QuickInspectorInterface *inspector, QWidget *parent = 0);
    ~QuickScenePreviewWidget();

    void restoreState(const QByteArray &state) Q_DECL_OVERRIDE;
    QByteArray saveState() const Q_DECL_OVERRIDE;

    void setSupportsCustomRenderModes(QuickInspectorInterface::Features supportedCustomRenderModes);

    QuickInspectorInterface::RenderMode customRenderMode() const;
    void setCustomRenderMode(QuickInspectorInterface::RenderMode customRenderMode);

private Q_SLOTS:
    void visualizeActionTriggered(QAction* current);
    void updateEffectiveGeometry();

private:
    void drawArrow(QPainter *p, QPointF first, QPointF second);
    void drawAnchor(QPainter *p, Qt::Orientation orientation, qreal ownAnchorLine, qreal offset,
                    const QString &label);

private:
    void drawDecoration(QPainter *p) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    struct {
        QToolBar *toolbarWidget;
        QComboBox *zoomCombobox;
        QActionGroup *visualizeGroup;
        QAction *visualizeClipping;
        QAction *visualizeOverdraw;
        QAction *visualizeBatches;
        QAction *visualizeChanges;
    } m_toolBar;

    QuickInspectorInterface *m_inspectorInterface;
    QuickItemGeometry m_effectiveGeometry; // scaled and translated
};
} // namespace GammaRay

#endif
