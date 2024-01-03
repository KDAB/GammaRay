/*
  quickscreengrabber.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    ItemOrLayoutFacade(QQuickItem *item); // krazy:exclude=explicit

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
    struct RenderInfo
    {
        // Keep in sync with QSGRendererInterface::GraphicsApi
        enum GraphicsApi
        {
            Unknown = 0,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            Software,
            OpenVG,
            OpenGL,
            Direct3D11,
            Vulkan,
            Metal,
            Null,
#else
            Software,
            OpenGL,
            Direct3D12
#endif
        };

        RenderInfo()
            : dpr(qQNaN())
        {
        }

        qreal dpr;
        QPoint windowPosition;
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
    void connectItemChanges(QQuickItem *item) const;
    void disconnectItemChanges(QQuickItem *item) const;
    void connectTopItemChanges(QQuickItem *item) const;
    void disconnectTopItemChanges(QQuickItem *item) const;

protected:
    QPointer<QQuickWindow> m_window;
    QPointer<QQuickItem> m_currentToplevelItem;
    ItemOrLayoutFacade m_currentItem;
    QuickDecorationsSettings m_settings;
    bool m_decorationsEnabled = true;
    QRectF m_userViewport;
    GrabbedFrame m_grabbedFrame;
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class UnsupportedScreenGrabber : public AbstractScreenGrabber
{
    Q_OBJECT
public:
    explicit UnsupportedScreenGrabber(QQuickWindow *window);
    ~UnsupportedScreenGrabber() override;

    void requestGrabWindow(const QRectF &userViewport) override;
    void drawDecorations() override;

private:
    void updateOverlay() override;

    QPointF m_lastItemPosition;
};
#endif

}

Q_DECLARE_METATYPE(GammaRay::GrabbedFrame)

#endif
