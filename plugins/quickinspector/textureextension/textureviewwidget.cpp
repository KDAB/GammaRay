/*
  textureviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textureviewwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <cmath>

using namespace GammaRay;

TextureViewWidget::TextureViewWidget(QWidget* parent)
    : RemoteViewWidget(parent)
    , m_visualizeTextureWaste(true)
{
}

TextureViewWidget::~TextureViewWidget()
{
}

QRect getBoundingRect(const QImage& image)
{
    int top = image.height(), bottom = 0, left = image.width(), right = 0;

    for(int y = 0; y < image.height(); y++)
        for(int x = 0; x < image.width(); x++) {
            if (qAlpha(image.pixel(x, y)) != 0) {
                top = std::min(top, y);
                bottom = std::max(bottom, y);
                left = std::min(left, x);
                right = std::max(right, x);
            }
        }
    return QRect(QPoint(left,top),QPoint(right,bottom));
}

void TextureViewWidget::drawPixelWasteDecoration(QPainter *p) const
{

    //For AtlasTiles analyze subrect, else analyze the whole image
    QImage analyzedTexture;
    QRect analyzedRect;
    auto atlasSubTile = frame().data().toRect();
    if (atlasSubTile.isValid()) { //Atlas-Case
        analyzedTexture = frame().image().copy(atlasSubTile);
        analyzedRect = atlasSubTile;
        analyzedRect = analyzedRect.adjusted(-1, -1, 1, 1);
    } else { //Whole-Texture-Case
        analyzedTexture = frame().image();
        analyzedRect = frame().image().rect();
    }

    //Calculate Waste
    auto boundingRect = getBoundingRect(analyzedTexture);
    const float imagePixelSize = (analyzedTexture.width() * analyzedTexture.height());
    const auto pixelWaste = 1.0 - ((boundingRect.height() * boundingRect.width()) / imagePixelSize);
    int pixelWastePercent = qRound(pixelWaste*100.0f);
    int pixelWasteInBytes = imagePixelSize * (boundingRect.height() * boundingRect.width()) * frame().image().depth()/8;

    emit textureWasteFound(pixelWastePercent, pixelWasteInBytes);

    //Draw Warning if more than 30% are wasted
    if (pixelWaste > 0.3) {
        emit textureInfoNecessary(true);
        p->save();
        auto scaleTransform = QTransform::fromScale(zoom(),zoom());
        p->setTransform(scaleTransform, true);

        //Draw Wasted Area
        QPen pen(Qt::red);
        pen.setCosmetic(true);
        p->setPen(pen);
        QBrush brush = QBrush(Qt::red, Qt::FDiagPattern);
        brush.setTransform(scaleTransform.inverted());
        p->setBrush(brush);
        auto viewRect = QPainterPath();
        viewRect.addRect(analyzedRect);
        auto innerRect = QPainterPath();
        boundingRect.translate(analyzedRect.x(), analyzedRect.y());
        innerRect.addRect(boundingRect);
        viewRect = viewRect.subtracted(innerRect);
        p->drawPath(viewRect);
    } else {
        emit textureInfoNecessary(false);
    }
}

void TextureViewWidget::drawActiveAtlasTile(QPainter *p) const
{
    auto rect = frame().data().toRect();
    if (!rect.isValid())
        return;

    rect = rect.adjusted(-1, -1, 1, 1);
    p->save();
    p->setTransform(QTransform().scale(zoom(), zoom()), true);
    QPen pen(Qt::red);
    pen.setCosmetic(true);
    p->setPen(pen);
    p->drawRect(rect);
    p->restore();
}

void TextureViewWidget::drawDecoration(QPainter *p)
{
    drawActiveAtlasTile(p);

    if (m_visualizeTextureWaste)
        drawPixelWasteDecoration(p);
}

void TextureViewWidget::setTextureWasteVisualizationEnabled(bool enabled)
{
    if (m_visualizeTextureWaste != enabled) {
        m_visualizeTextureWaste = enabled;
        update();
    }
}

