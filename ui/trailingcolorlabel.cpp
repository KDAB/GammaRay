#include "trailingcolorlabel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QDebug>

namespace GammaRay {

TrailingColorLabel::TrailingColorLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setMinimumSize(QSize(145, 30));
    m_pickedColor = qRgba(0,0,0,0);
    setHidden(true);
}

void TrailingColorLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter *p = new QPainter(this);
    p->setPen(QPen(Qt::NoPen));
    const auto palette = this->style()->standardPalette();
    p->setBrush(palette.base());
    p->drawRect(0,0,145,30);
    p->setPen(QPen(palette.color(QPalette::Normal, QPalette::Text)));

    p->drawText(QRect(20,0,30,30), Qt::AlignVCenter | Qt::AlignRight, QString::number(qRed(m_pickedColor)));
    p->drawText(QRect(50,0,30,30), Qt::AlignVCenter | Qt::AlignRight, QString::number(qGreen(m_pickedColor)));
    p->drawText(QRect(80,0,30,30), Qt::AlignVCenter | Qt::AlignRight, QString::number(qBlue(m_pickedColor)));
    //alpha
    p->setPen(QPen(palette.color(QPalette::Disabled, QPalette::Text)));
    p->drawText(QRect(113,0,30,30), Qt::AlignVCenter | Qt::AlignLeft, "|");
    p->drawText(QRect(110,0,30,30), Qt::AlignVCenter | Qt::AlignRight, QString::number(qAlpha(m_pickedColor)));

    { // Color bg-pattern
    QBrush brush;
    QPixmap bgPattern(20, 20);
    bgPattern.fill(Qt::lightGray);
    QPainter bgPainter(&bgPattern);
    bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
    bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
    brush.setTexture(bgPattern);
    p->setBrush(brush);
    p->drawRect(5,5,20,20);
    }

    QColor color = QColor::fromRgba(m_pickedColor);
    p->setBrush(QBrush(color));
    p->setPen(QPen(Qt::black));
    p->drawRect(5,5,20,20);
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
