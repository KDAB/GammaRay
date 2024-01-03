/*
  overlaywidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_OVERLAYWIDGET_H
#define GAMMARAY_WIDGETINSPECTOR_OVERLAYWIDGET_H

#include <QLayout>
#include <QPainterPath>
#include <QPointer>
#include <QWidget>

namespace GammaRay {

class WidgetOrLayoutFacade
{
public:
    WidgetOrLayoutFacade()
        : m_object(nullptr)
    {
    }
    WidgetOrLayoutFacade(QWidget *widget)
        : m_object(widget)
    {
    } // krazy:exclude=explicit
    WidgetOrLayoutFacade(QLayout *layout)
        : m_object(layout)
    {
    } // krazy:exclude=explicit

    /// Get either the layout of the widget or the this-pointer
    inline QLayout *layout() const
    {
        return isLayout() ? asLayout() : asWidget()->layout();
    }

    /// Get either the parent widget of the layout or the this-pointer
    QWidget *widget() const
    {
        return isLayout() ? asLayout()->parentWidget() : asWidget();
    }

    QRect geometry() const
    {
        return isLayout() ? asLayout()->geometry() : asWidget()->geometry();
    }

    bool isVisible() const
    {
        return widget() ? widget()->isVisible() && !widget()->isHidden() : false;
    }

    QPoint pos() const
    {
        return isLayout() ? asLayout()->geometry().topLeft() : QPoint(0, 0);
    }

    inline bool isNull() const
    {
        return !m_object;
    }
    inline QObject *data()
    {
        return m_object;
    }
    inline QObject *operator->() const
    {
        Q_ASSERT(!isNull());
        return m_object;
    }
    inline void clear()
    {
        m_object = nullptr;
    }

private:
    inline bool isLayout() const
    {
        Q_ASSERT(!isNull());
        return qobject_cast<QLayout *>(m_object);
    }
    inline QLayout *asLayout() const
    {
        return static_cast<QLayout *>(m_object.data());
    }
    inline QWidget *asWidget() const
    {
        return static_cast<QWidget *>(m_object.data());
    }

    QPointer<QObject> m_object;
};

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    OverlayWidget();

    /**
     * Place the overlay on @p item
     *
     * @param item The overlay can be cover a widget or a layout
     */
    void placeOn(const WidgetOrLayoutFacade &item);

    bool eventFilter(QObject *receiver, QEvent *event) override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void resizeOverlay();
    void updatePositions();

    QWidget *m_currentToplevelWidget;
    WidgetOrLayoutFacade m_currentItem;
    QRect m_outerRect;
    QColor m_outerRectColor;

    QPainterPath m_layoutPath;
    bool m_drawLayoutOutlineOnly;
};
}

#endif
