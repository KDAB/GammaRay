/*
  overlaywidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "overlaywidget.h"

#include <QDebug>
#include <QDialog>
#include <QEvent>
#include <QLayout>
#include <QPainter>
#include <QSplitter>

using namespace GammaRay;

static QWidget *toplevelWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    QWidget *parent = widget;
    auto isTopLevel = [](QWidget *widget) {
        return widget->isWindow();
    };
    auto lastSuitableParent = parent;
    while (parent->parentWidget() && !isTopLevel(parent->parentWidget()) && !isTopLevel(parent)) {
        parent = parent->parentWidget();

        // don't pick parents that can't take the overlay as a children
        if (!qobject_cast<QSplitter *>(parent)) {
            lastSuitableParent = parent;
        }
    }

    return lastSuitableParent;
}

OverlayWidget::OverlayWidget()
    : m_currentToplevelWidget(nullptr)
    , m_drawLayoutOutlineOnly(true)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
}

void OverlayWidget::placeOn(const WidgetOrLayoutFacade &item)
{
    if (item.isNull()) {
        if (!m_currentItem.isNull())
            m_currentItem->removeEventFilter(this);

        if (m_currentToplevelWidget)
            m_currentToplevelWidget->removeEventFilter(this);

        m_currentToplevelWidget = nullptr;
        m_currentItem.clear();
        m_outerRect = QRect();
        m_layoutPath = QPainterPath();

        update();
        return;
    }

    if (!m_currentItem.isNull())
        m_currentItem->removeEventFilter(this);

    m_currentItem = item;

    QWidget *toplevel = toplevelWidget(item.widget());
    Q_ASSERT(toplevel);

    if (toplevel != m_currentToplevelWidget) {
        if (m_currentToplevelWidget)
            m_currentToplevelWidget->removeEventFilter(this);

        m_currentToplevelWidget = toplevel;

        setParent(toplevel);
        move(0, 0);
        resize(toplevel->size());

        m_currentToplevelWidget->installEventFilter(this);

        show();
    }

    m_currentItem->installEventFilter(this);

    updatePositions();
}

bool OverlayWidget::eventFilter(QObject *receiver, QEvent *event)
{
    if (!m_currentItem.isNull() && m_currentToplevelWidget != m_currentItem.widget()->window()) { // detect (un)docking
        placeOn(m_currentItem);
        return false;
    }

    if (receiver == m_currentItem.data()) {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Move || event->type() == QEvent::Show || event->type() == QEvent::Hide) {
            resizeOverlay();
            updatePositions();
        }
    } else if (receiver == m_currentToplevelWidget) {
        if (event->type() == QEvent::Resize) {
            resizeOverlay();
            updatePositions();
        }
    }

    return false;
}

void OverlayWidget::updatePositions()
{
    if (m_currentItem.isNull() || !m_currentToplevelWidget)
        return;

    if (!m_currentItem.isVisible())
        m_outerRectColor = Qt::green;
    else
        m_outerRectColor = Qt::red;

    const QPoint parentPos = m_currentItem.widget()->mapTo(m_currentToplevelWidget, m_currentItem.pos());
    m_outerRect = QRect(parentPos.x(), parentPos.y(),
                        m_currentItem.geometry().width(),
                        m_currentItem.geometry().height())
                      .adjusted(0, 0, -1, -1);

    m_layoutPath = QPainterPath();

    if (m_currentItem.layout()
        && qstrcmp(m_currentItem.layout()->metaObject()->className(),
                   "QMainWindowLayout")
            != 0) {
        const QRect layoutGeometry = m_currentItem.layout()->geometry();

        const QRect mappedOuterRect = QRect(m_currentItem.widget()->mapTo(m_currentToplevelWidget,
                                                                          layoutGeometry.topLeft()),
                                            layoutGeometry.size());

        QPainterPath outerPath;
        outerPath.addRect(mappedOuterRect.adjusted(1, 1, -2, -2));

        QPainterPath innerPath;
        for (int i = 0; i < m_currentItem.layout()->count(); ++i) {
            QLayoutItem *item = m_currentItem.layout()->itemAt(i);
            if (item->widget() && !item->widget()->isVisible())
                continue;
            const QRect mappedInnerRect = QRect(m_currentItem.widget()->mapTo(m_currentToplevelWidget,
                                                                              item->geometry().topLeft()),
                                                item->geometry().size());
            innerPath.addRect(mappedInnerRect);
        }

        m_layoutPath.setFillRule(Qt::OddEvenFill);
        m_layoutPath = outerPath.subtracted(innerPath);

        if (m_layoutPath.isEmpty()) {
            m_layoutPath = std::move(outerPath);
            m_layoutPath.addPath(innerPath);
            m_drawLayoutOutlineOnly = true;
        } else {
            m_drawLayoutOutlineOnly = false;
        }
    }

    update();
}

void OverlayWidget::resizeOverlay()
{
    if (m_currentToplevelWidget) {
        move(0, 0);
        resize(m_currentToplevelWidget->size());
    }
}

void OverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(m_outerRectColor);
    p.drawRect(m_outerRect);

    QBrush brush(Qt::BDiagPattern);
    brush.setColor(Qt::blue);

    if (!m_drawLayoutOutlineOnly)
        p.fillPath(m_layoutPath, brush);

    p.setPen(Qt::blue);
    p.drawPath(m_layoutPath);
}
