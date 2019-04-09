/*
  trailingcolorlabel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "trailingcolorlabel.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>


namespace GammaRay {

TrailingColorLabel::TrailingColorLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pickedColor = qRgba(0,0,0,0);
    setHidden(true);
}

void TrailingColorLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter *p = new QPainter(this);


    QFontMetrics metrics(p->font());

    const int margin = 5;
    const int squaresize = 20;
    const int height = 30;
    const int numberGroupWidth = metrics.width(QStringLiteral("  000"));
    const int width = margin + squaresize + margin + 4 * numberGroupWidth + margin;
    setMinimumSize(QSize(width + 1, height + 1));


    p->setPen(QPen(Qt::lightGray));
    p->setBrush(palette().base());
    p->drawRect(0, 0, width, height);
    p->setPen(QPen(palette().color(QPalette::Normal, QPalette::Text)));

    p->drawText(QRect(margin + squaresize + margin + 0 * numberGroupWidth, 0, numberGroupWidth, height),
                Qt::AlignVCenter | Qt::AlignRight,
                QString::number(qRed(m_pickedColor)));
    p->drawText(QRect(margin + squaresize + margin + 1 * numberGroupWidth, 0, numberGroupWidth, height),
                Qt::AlignVCenter | Qt::AlignRight,
                QString::number(qGreen(m_pickedColor)));
    p->drawText(QRect(margin + squaresize + margin + 2 * numberGroupWidth, 0, numberGroupWidth, height),
                Qt::AlignVCenter | Qt::AlignRight,
                QString::number(qBlue(m_pickedColor)));
    //alpha
    p->setPen(QPen(palette().color(QPalette::Disabled, QPalette::Text)));
    p->drawText(QRect(margin + squaresize + margin + 3 * numberGroupWidth + 2, 0, numberGroupWidth, height),
                Qt::AlignVCenter | Qt::AlignLeft,
                "|");
    p->drawText(QRect(margin + squaresize + margin + 3 * numberGroupWidth, 0, numberGroupWidth, height),
                Qt::AlignVCenter | Qt::AlignRight,
                QString::number(qAlpha(m_pickedColor)));
    { // Color bg-pattern
    QBrush brush;
    QPixmap bgPattern(squaresize, squaresize);
    bgPattern.fill(Qt::lightGray);
    QPainter bgPainter(&bgPattern);
    bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
    bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
    brush.setTexture(bgPattern);
    p->setBrush(brush);
    p->drawRect(margin, margin, squaresize, squaresize);
    }

    QColor color = QColor::fromRgba(m_pickedColor);
    p->setBrush(QBrush(color));
    p->setPen(QPen(Qt::black));
    p->drawRect(margin, margin, squaresize, squaresize);
}

QColor TrailingColorLabel::pickedColor() const
{
    return QColor::fromRgba(m_pickedColor);
}

void TrailingColorLabel::setPickedColor(QRgb color)
{
    m_pickedColor = color;
}

}
