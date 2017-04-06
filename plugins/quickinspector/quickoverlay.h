/*
  quickoverlay.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKOVERLAY_H
#define GAMMARAY_QUICKINSPECTOR_QUICKOVERLAY_H

#include <QObject>
#include <QPointer>
#include <QQuickItem>

#include "quickdecorationsdrawer.h"

QT_BEGIN_NAMESPACE
class QQuickWindow;
QT_END_NAMESPACE

namespace GammaRay {

class ItemOrLayoutFacade
{
public:
    ItemOrLayoutFacade();
    ItemOrLayoutFacade(QQuickItem *item); //krazy:exclude=explicit

    /// Get either the layout of the widget or the this-pointer
    QQuickItem *layout() const;

    /// Get either the parent widget of the layout or the this-pointer
    QQuickItem *item() const;

    QRectF geometry() const;
    bool isVisible() const;
    QPointF pos() const;

    inline bool isNull() const { return !m_object; }
    inline QQuickItem* data() { return m_object; }
    inline QQuickItem* operator->() const { Q_ASSERT(!isNull()); return m_object; }
    inline void clear() { m_object = nullptr; }

private:
    bool isLayout() const;
    inline QQuickItem *asLayout() const { return m_object; }
    inline QQuickItem *asItem() const { return m_object; }

    QPointer<QQuickItem> m_object;
};

class GrabedFrame {
public:
    QImage image;
    QTransform transform;
    QRectF itemsGeometryRect;
    QVector<QuickItemGeometry> itemsGeometry;
};

class QuickOverlay : public QObject
{
    Q_OBJECT

public:
    QuickOverlay();

    QQuickWindow *window() const;
    void setWindow(QQuickWindow *window);

    QuickDecorationsSettings settings() const;
    void setSettings(const QuickDecorationsSettings &settings);

    bool decorationsEnabled() const;
    void setDecorationsEnabled(bool enabled);

    /**
     * Place the overlay on @p item
     *
     * @param item The overlay can be cover a widget or a layout of the current window
     */
    void placeOn(const ItemOrLayoutFacade &item);

public slots:
    void requestGrabWindow();

signals:
    void sceneChanged();
    void sceneGrabbed(const GammaRay::GrabedFrame &frame);

private:
    void windowAfterSynchronizing();
    void windowAfterRendering();
    void gatherRenderInfo();
    void drawDecorations();
    void updateOverlay();
    void itemParentChanged(QQuickItem *parent);
    void itemWindowChanged(QQuickWindow *window);
    void connectItemChanges(QQuickItem *item);
    void disconnectItemChanges(QQuickItem *item);
    void connectTopItemChanges(QQuickItem *item);
    void disconnectTopItemChanges(QQuickItem *item);
    QuickItemGeometry initFromItem(QQuickItem *item) const;

    QPointer<QQuickWindow> m_window;
    QPointer<QQuickItem> m_currentToplevelItem;
    ItemOrLayoutFacade m_currentItem;
    QuickDecorationsSettings m_settings;
    bool m_isGrabbingMode;
    bool m_decorationsEnabled;
    GrabedFrame m_grabedFrame;
    struct RenderInfo {
        // Keep in sync with QSGRendererInterface::GraphicsApi
        enum GraphicsApi {
            Unknown,
            Software,
            OpenGL,
            Direct3D12
        };

        RenderInfo()
            : dpr(qQNaN())
            , graphicsApi(Unknown)
        { }

        qreal dpr;
        QSize windowSize;
        GraphicsApi graphicsApi;
    } m_renderInfo;

private slots:
    void setIsGrabbingMode(bool isGrabbingMode);
};
}

Q_DECLARE_METATYPE(GammaRay::GrabedFrame)

#endif
