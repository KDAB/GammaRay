/*
  widgetremoteview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "widgetremoteview.h"
#include "widgetinspectorinterface.h"

#include <compat/qasconst.h>

#include <QPainter>
#include <QVector2D>

using namespace GammaRay;

WidgetRemoteView::WidgetRemoteView(QWidget *parent)
    : RemoteViewWidget(parent)
    , m_tabFocusEnabled(false)
{
}

WidgetRemoteView::~WidgetRemoteView() = default;

void WidgetRemoteView::setTabFocusOverlayEnabled(bool enabled)
{
    m_tabFocusEnabled = enabled;
    update();
}

static void drawArrow(QPainter *p, QPointF first, QPointF second)
{
    p->drawLine(first, second);
    QPointF vector(second - first);
    QTransform m;
    m.rotate(30);
    QVector2D v1 = QVector2D(m.map(vector)).normalized() * 10;
    m.rotate(-60);
    QVector2D v2 = QVector2D(m.map(vector)).normalized() * 10;
    p->drawLine(second, second - v1.toPointF());
    p->drawLine(second, second - v2.toPointF());
}

void WidgetRemoteView::drawDecoration(QPainter *p)
{
    if (!m_tabFocusEnabled)
        return;

    const auto data = frame().data.value<WidgetFrameData>();
    if (data.tabFocusRects.size() < 2)
        return;

    p->save();
    p->setPen(Qt::darkGreen);
    for (const auto &rect : data.tabFocusRects)
        p->drawRect(mapFromSource(rect));

    QVector<QLineF> lines;
    lines.reserve(data.tabFocusRects.size());
    for (int i = 0; i < data.tabFocusRects.size() - 1; ++i) {
        const auto r1 = mapFromSource(data.tabFocusRects.at(i));
        const auto r2 = mapFromSource(data.tabFocusRects.at(i + 1));

        p->setPen(Qt::green);
        QLineF l(r1.center(), r2.center());
        for (const auto &prevLine : qAsConst(lines)) {
            QPointF pnt;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
            if (l.intersect(prevLine, &pnt) == QLineF::BoundedIntersection && pnt != l.p1() && pnt != l.p2()) {
#else
            if (l.intersects(prevLine, &pnt) == QLineF::BoundedIntersection && pnt != l.p1() && pnt != l.p2()) {
#endif
                p->setPen(Qt::red);
                break;
            }
        }
        lines.push_back(l);
        drawArrow(p, r1.center(), r2.center());
    }
    p->restore();
}
