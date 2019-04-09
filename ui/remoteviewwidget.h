/*
  remoteviewwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_REMOTEVIEWWIDGET_H
#define GAMMARAY_REMOTEVIEWWIDGET_H

#include "gammaray_ui_export.h"

#include <common/objectid.h>
#include <common/remoteviewframe.h>

#include <QElapsedTimer>
#include <QPointer>
#include <QTouchEvent>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QActionGroup;
class QStandardItemModel;
class QModelIndex;
class QEvent;
class QTouchEvent;
QT_END_NAMESPACE

namespace GammaRay {
class RemoteViewInterface;
class ObjectIdsFilterProxyModel;
class VisibilityFilterProxyModel;
class TrailingColorLabel;

/** Widget showing remote screen content and providing both visual inspection
 *  capabilities as well as input redirection.
 *
 *  Can be sub-classed to support e.g. custom element decoration.
 */
class GAMMARAY_UI_EXPORT RemoteViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteViewWidget(QWidget *parent = nullptr);
    ~RemoteViewWidget() override;

    /// Set the object name for connecting to the server.
    void setName(const QString &name);

    enum InteractionMode {
        NoInteraction = 0, ///< disable all
        ViewInteraction = 1, ///< panning, zooming, etc
        Measuring = 2,
        InputRedirection = 4,
        ElementPicking = 8,
        ColorPicking = 16
    };
    InteractionMode interactionMode() const;
    void setInteractionMode(InteractionMode mode);

    Q_DECLARE_FLAGS(InteractionModes, InteractionMode)
    InteractionModes supportedInteractionModes() const;
    void setSupportedInteractionModes(InteractionModes modes);

    /// Returns the current zoom level
    double zoom() const;
    /// Returns the index of the current zoom level, useful for updating a combo box using the zoomLevelModel.
    int zoomLevelIndex() const;
    /// Model containing the supported zoom levels, for use with a combo box
    QAbstractItemModel *zoomLevelModel() const;

    /// Set the message that is shown when remote view is unavailable.
    void setUnavailableText(const QString &msg);

    /// Action group containing all interaction mode switch actions
    QActionGroup *interactionModeActions() const;
    QAction *zoomOutAction() const;
    QAction *zoomInAction() const;

    QAbstractItemModel *pickSourceModel() const;
    void setPickSourceModel(QAbstractItemModel *sourceModel);

    Q_INVOKABLE virtual void restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

    int flagRole() const;
    void setFlagRole(int flagRole);

    int invisibleMask() const;
    void setInvisibleMask(int invisibleMask);

    bool hasValidFrame() const;
    bool hasValidCompleteFrame() const;

public slots:
    /// Clears the current view content.
    void reset();
    /// Sets the zoom level to the closest level to @p zoom.
    void setZoom(double zoom);
    /// Sets the zoom level to the level at @p index in the zoom level model.
    void setZoomLevel(int index);
    void zoomIn();
    void zoomOut();
    void fitToView();
    void centerView();

signals:
    void zoomChanged();
    void zoomLevelChanged(int zoomLevelIndex);
    void interactionModeChanged();
    void stateChanged();
    void frameChanged();

protected:
    /** Current frame data. */
    const RemoteViewFrame &frame() const;
    RemoteViewInterface *remoteViewInterface() const;

    /** Override this to draw element decorations.
     *  @P p is translated to that 0,0 is the top left corner of the source image, but not scaled
     */
    virtual void drawDecoration(QPainter *p);

    // translate from view coordinates to source coordinates
    QPoint mapToSource(QPoint pos) const;
    QPointF mapToSource(QPointF pos) const;
    QRectF mapToSource(const QRectF &r) const;
    // translates from source coordinates to view coordinates
    QPoint mapFromSource(QPoint pos) const;
    QPointF mapFromSource(QPointF pos) const;
    QRectF mapFromSource(const QRect &rect) const;
    // translate from view to source coordinates
    QTouchEvent::TouchPoint mapToSource(const QTouchEvent::TouchPoint &point);

    void restoreState(QDataStream &stream);
    void saveState(QDataStream &stream) const;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;


    bool eventFilter(QObject *receiver, QEvent *event) override;
    bool event(QEvent *event) override;

private:
    void setupActions();
    void updateActions();

    void drawBackground(QPainter *p);
    void drawRuler(QPainter *p);
    void drawFPS(QPainter *p);
    int sourceTickLabelDistance(int viewDistance);
    int viewTickLabelDistance() const;
    void drawMeasureOverlay(QPainter *p);
    void drawMeasurementLabel(QPainter *p, QPoint pos, QPoint dir, const QString &text);

    void clampPanPosition();

    void sendMouseEvent(QMouseEvent *event);
    void sendKeyEvent(QKeyEvent *event);
    void sendWheelEvent(QWheelEvent *event);
    void sendTouchEvent(QTouchEvent *event);

    // size of the content area, ie. excluding the rulers
    int contentWidth() const;
    int contentHeight() const;

    // size of the rulers
    int horizontalRulerHeight() const;
    int verticalRulerWidth() const;

    void updatePickerVisibility() const;
    void pickColor() const;

private slots:
    void interactionActionTriggered(QAction *action);
    void pickElementId(const QModelIndex &index);
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);
    void frameUpdated(const GammaRay::RemoteViewFrame &frame);
    void enableFPS(const bool showFPS);
    void updateUserViewport();

private:
    RemoteViewFrame m_frame;
    QBrush m_activeBackgroundBrush;
    QBrush m_inactiveBackgroundBrush;
    QVector<double> m_zoomLevels;
    QStandardItemModel *m_zoomLevelModel;
    QString m_unavailableText;
    QVector<int> m_tickLabelDists;
    QActionGroup *m_interactionModeActions;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_toggleFPSAction;
    QPointer<RemoteViewInterface> m_interface;
    TrailingColorLabel *m_trailingColorLabel;
    double m_zoom;
    int m_x; // view translation before zoom
    int m_y;
    InteractionMode m_interactionMode;
    InteractionModes m_supportedInteractionModes;
    QPoint m_mouseDownPosition; // semantics depend on interaction mode
    QPointF m_currentMousePosition; // in view coordinates
    QPoint m_measurementStartPosition; // in source coordinates
    QPoint m_measurementEndPosition; // in source coordinates
    bool m_hasMeasurement;
    ObjectIdsFilterProxyModel *m_pickProxyModel;
    VisibilityFilterProxyModel *m_invisibleItemsProxyModel;
    bool m_initialZoomDone;
    bool m_extraViewportUpdateNeeded;
    int m_flagRole;
    int m_invisibleMask;
    QElapsedTimer m_fpsTimer;
    bool m_showFps;
    qreal m_fps;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::RemoteViewWidget::InteractionModes)

#endif // GAMMARAY_REMOTEVIEWWIDGET_H
