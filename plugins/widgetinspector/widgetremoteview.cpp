/*
  widgetremoteview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "widgetremoteview.h"
#include "widgetinspectorinterface.h"

#include <compat/qasconst.h>

#include <QPainter>
#include <QVector2D>

using namespace GammaRay;

WidgetRemoteView::WidgetRemoteView(QWidget* parent)
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
    QMatrix m;
    m.rotate(30);
    QVector2D v1 = QVector2D(m.map(vector)).normalized() * 10;
    m.rotate(-60);
    QVector2D v2 = QVector2D(m.map(vector)).normalized() * 10;
    p->drawLine(second, second - v1.toPointF());
    p->drawLine(second, second - v2.toPointF());
}

void WidgetRemoteView::drawDecoration(QPainter* p)
{
    if (!m_tabFocusEnabled)
        return;

    const auto data = frame().data().value<WidgetFrameData>();
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
            if (l.intersect(prevLine, &pnt) == QLineF::BoundedIntersection && pnt != l.p1() && pnt != l.p2()) {
                p->setPen(Qt::red);
                break;
            }
        }
        lines.push_back(l);
        drawArrow(p, r1.center(), r2.center());
    }
    p->restore();
}
