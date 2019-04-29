/*
  quickscreengrabber.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCREENGRABBER_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCREENGRABBER_H

#include "quickdecorationsdrawer.h"

#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QMutex>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickWindow;
#ifndef QT_NO_OPENGL
class QOpenGLPaintDevice;
#endif
class QSGSoftwareRenderer;
QT_END_NAMESPACE

namespace GammaRay {

class ItemOrLayoutFacade
{
public:
    ItemOrLayoutFacade() = default;
    ItemOrLayoutFacade(QQuickItem *item); //krazy:exclude=explicit

    /// Get either the layout of the widget or the this-pointer
    QQuickItem *layout() const;

    /// Get either the parent widget of the layout or the this-pointer
    QQuickItem *item() const;

    QRectF geometry() const;
    bool isVisible() const;
    QPointF pos() const;

    inline bool isNull() const
    {
        return !m_object;
    }

    inline QQuickItem *data()
    {
        return m_object;
    }

    inline QQuickItem *operator->() const
    {
        Q_ASSERT(!isNull());
        return m_object;
    }

    inline void clear()
    {
        m_object = nullptr;
    }

private:
    bool isLayout() const;
    inline QQuickItem *asLayout() const
    {
        return m_object;
    }

    inline QQuickItem *asItem() const
    {
        return m_object;
    }

    QPointer<QQuickItem> m_object;
};

class GrabbedFrame
{
public:
    QImage image;
    QTransform transform;
    QRectF itemsGeometryRect;
    QVector<QuickItemGeometry> itemsGeometry;
};

class AbstractScreenGrabber : public QObject
{
    Q_OBJECT
public:
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
        {
        }

        qreal dpr;
        QSize windowSize;
        GraphicsApi graphicsApi = Unknown;
    };

    explicit AbstractScreenGrabber(QQuickWindow *window);
    ~AbstractScreenGrabber() override;

    static RenderInfo::GraphicsApi graphicsApiFor(QQuickWindow *window);
    static std::unique_ptr<AbstractScreenGrabber> get(QQuickWindow *window);

    QQuickWindow *window() const;

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

    virtual void requestGrabWindow(const QRectF &userViewport) = 0;

signals:
    void grabberReadyChanged(bool ready);
    void sceneChanged();
    void sceneGrabbed(const GammaRay::GrabbedFrame &frame);

protected:
    void doDrawDecorations(QPainter &painter);
    void gatherRenderInfo();

    virtual void drawDecorations() = 0;

    virtual void updateOverlay();
    static QuickItemGeometry initFromItem(QQuickItem *item);

private:
    void itemParentChanged(QQuickItem *parent);
    void itemWindowChanged(QQuickWindow *window);
    void connectItemChanges(QQuickItem *item);
    void disconnectItemChanges(QQuickItem *item);
    void connectTopItemChanges(QQuickItem *item);
    void disconnectTopItemChanges(QQuickItem *item);

protected:
    QPointer<QQuickWindow> m_window;
    QPointer<QQuickItem> m_currentToplevelItem;
    ItemOrLayoutFacade m_currentItem;
    QuickDecorationsSettings m_settings;
    bool m_decorationsEnabled = true;
    QRectF m_userViewport;
    GrabbedFrame m_grabbedFrame;
    QMetaMethod m_sceneChanged;
    QMetaMethod m_sceneGrabbed;
    RenderInfo m_renderInfo;
};

#ifndef QT_NO_OPENGL
class OpenGLScreenGrabber : public AbstractScreenGrabber
{
    Q_OBJECT
public:
    explicit OpenGLScreenGrabber(QQuickWindow *window);
    ~OpenGLScreenGrabber() override;

    void requestGrabWindow(const QRectF &userViewport) override;
    void drawDecorations() override;

private:
    void setGrabbingMode(bool isGrabbingMode, const QRectF &userViewport);
    void windowAfterSynchronizing();
    void windowAfterRendering();

    bool m_isGrabbing;
    QMutex m_mutex;
};
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
class SoftwareScreenGrabber : public AbstractScreenGrabber
{
    Q_OBJECT
public:
    explicit SoftwareScreenGrabber(QQuickWindow *window);
    ~SoftwareScreenGrabber() override;

    void requestGrabWindow(const QRectF &userViewport) override;
    void drawDecorations() override;

private:
    void windowAfterRendering();
    void windowBeforeRendering();
    void updateOverlay() override;

    QSGSoftwareRenderer *softwareRenderer() const;

    bool m_isGrabbing = false;
    QPointF m_lastItemPosition;
};
#endif

}

Q_DECLARE_METATYPE(GammaRay::GrabbedFrame)

#endif
