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
    , m_visualizeTextureProblems(true)
    , m_pixelWasteInPercent(0)
    , m_pixelWasteInBytes(0)
    , m_horizontalBorderImageSavings(0)
{
    connect(this, SIGNAL(frameChanged()), this, SLOT(analyzeImageFlaws()));
}

TextureViewWidget::~TextureViewWidget()
{
}

void TextureViewWidget::drawPixelWasteDecoration(QPainter *p) const
{
    //Draw Warning if more than 30% or 1KB are wasted
    if (m_pixelWasteInPercent < transparencyWasteLimitInPercent
        && m_pixelWasteInBytes < transparencyWasteLimitInBytes)
        return;

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
    viewRect.addRect(m_analyzedRect);
    auto innerRect = QPainterPath();
    auto translatedBoundingRect = m_opaqueBoundingRect.translated(m_analyzedRect.x(), m_analyzedRect.y());
    innerRect.addRect(translatedBoundingRect);
    viewRect = viewRect.subtracted(innerRect);
    p->drawPath(viewRect);
    p->restore();
}

void TextureViewWidget::drawBorderImageCutouts(QPainter *p) const
{
    if (m_horizontalBorderImageSavings > minimumBorderImageSavingsPercent) {
        p->save();
        auto scaleTransform = QTransform::fromScale(zoom(),zoom());
        p->setTransform(scaleTransform, true);

        QPen pen(Qt::white);
        pen.setCosmetic(true);
        p->setPen(pen);
        QBrush brush = QBrush(Qt::white, Qt::FDiagPattern);
        brush.setTransform(scaleTransform.inverted());
        p->setBrush(brush);
        p->drawRect(m_horizontalBorderRectMidCut.translated(m_analyzedRect.x(), m_analyzedRect.y()));
        p->restore();
    }

    if (m_verticalBorderImageSavings > minimumBorderImageSavingsPercent) {
        p->save();
        auto scaleTransform = QTransform::fromScale(zoom(),zoom());
        p->setTransform(scaleTransform, true);

        QPen pen(Qt::white);
        pen.setCosmetic(true);
        p->setPen(pen);
        QBrush brush = QBrush(Qt::white, Qt::FDiagPattern);
        brush.setTransform(scaleTransform.inverted());
        p->setBrush(brush);
        p->drawRect(m_verticalBorderRectMidCut.translated(m_analyzedRect.x(), m_analyzedRect.y()));
        p->restore();
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

    if (m_visualizeTextureProblems){
        drawBorderImageCutouts(p);
        drawPixelWasteDecoration(p);
    }
}

void TextureViewWidget::setTextureWasteVisualizationEnabled(bool enabled)
{
    if (m_visualizeTextureProblems != enabled) {
        m_visualizeTextureProblems = enabled;
        update();
    }
}

void TextureViewWidget::analyzeImageFlaws()
{
    emit textureInfoNecessary(false);

    if (frame().image().isNull())
        return;

    // For AtlasTiles analyze subrect, else analyze the whole image
    QImage analyzedTexture;
    QRect analyzedRect;
    auto atlasSubTile = frame().data().toRect();
    if (atlasSubTile.isValid()) { //Atlas-Case
        analyzedTexture = frame().image().copy(atlasSubTile);
        analyzedRect = atlasSubTile;
        analyzedRect = analyzedRect.adjusted(-1, -1, 1, 1);
    } else { // Whole-Texture-Case
        analyzedTexture = frame().image();
        analyzedRect = frame().image().rect();
    }

    m_analyzedRect = analyzedRect;

    QRgb possibleSingularColor = analyzedTexture.pixel(0, 0);
    ImageFlags imageFlags = ImageFlag::FullyTransparent | ImageFlag::FullyOpaque | ImageFlag::Unicolor;
    int top = analyzedTexture.height(), bottom = 0, left = analyzedTexture.width(), right = 0;

    for(int y = 0; y < analyzedTexture.height(); y++) {
        for(int x = 0; x < analyzedTexture.width(); x++) {
            auto pixel = analyzedTexture.pixel(x, y);

            if (Q_UNLIKELY(imageFlags.testFlag(ImageFlag::Unicolor)) && (possibleSingularColor != pixel))
                imageFlags &=~ ImageFlag::Unicolor;

            if (Q_UNLIKELY(imageFlags.testFlag(ImageFlag::FullyOpaque)) && (qAlpha(pixel) < 255))
                imageFlags &=~ ImageFlag::FullyOpaque;

            if (qAlpha(pixel) != 0) {
                imageFlags &=~ ImageFlag::FullyTransparent;
                top = std::min(top, y);
                bottom = std::max(bottom, y);
                left = std::min(left, x);
                right = std::max(right, x);
            }
        }
    }

    m_opaqueBoundingRect = QRect(QPoint(left, top), QPoint(right, bottom));

    // Calculate Waste
    const float imagePixelSize = (analyzedTexture.width() * analyzedTexture.height());
    const auto pixelWaste = 1.0 - ((m_opaqueBoundingRect.height() * m_opaqueBoundingRect.width()) / imagePixelSize);
    m_pixelWasteInPercent = qRound(pixelWaste * 100.0f);
    m_pixelWasteInBytes = (imagePixelSize - (m_opaqueBoundingRect.height() * m_opaqueBoundingRect.width())) * frame().image().depth() / 8;


    // Emit all possible Problems so far
    auto hasTextureWasteProblem = (m_pixelWasteInPercent > transparencyWasteLimitInPercent || m_pixelWasteInBytes > transparencyWasteLimitInBytes);
    emit textureWasteFound(hasTextureWasteProblem, m_pixelWasteInPercent, m_pixelWasteInBytes);
    if (hasTextureWasteProblem) imageFlags |= ImageFlag::TextureWaste;
    emit textureIsUnicolor(imageFlags.testFlag(ImageFlag::Unicolor));
    emit textureIsFullyTransparent(imageFlags.testFlag(ImageFlag::FullyTransparent));
    QVector<QImage::Format> commonFormatsWithAlpha = {QImage::Format_ARGB32, QImage::Format_ARGB32_Premultiplied};
    emit textureHasUselessAlpha(imageFlags.testFlag(ImageFlag::FullyOpaque) && commonFormatsWithAlpha.contains(frame().image().format()));

    // Border Image checks
    // horizontal mid slices
    auto textureWidth = analyzedTexture.width();
    auto textureHeight = analyzedTexture.height();
    auto midCol = textureWidth / 2;
    auto leftCol = midCol;
    auto breakout = false;
    while ((leftCol > 0) && !breakout) {
        leftCol--;
        for (int row = 0; row < textureHeight; row++) {
            if (analyzedTexture.pixel(leftCol, row) != analyzedTexture.pixel(midCol, row)) {
                breakout = true;
                break;
            }
        }
    }
    auto rightCol = midCol;
    breakout = false;
    while ((rightCol < textureWidth - 1) && !breakout) {
        rightCol++;
        for (int row = 0; row < textureHeight; row++) {
            if (analyzedTexture.pixel(rightCol, row) != analyzedTexture.pixel(midCol, row)) {
                breakout = true;
                break;
            }
        }
    }
    m_horizontalBorderImageSavings = qRound(((rightCol - leftCol) * textureHeight) / imagePixelSize * 100) ;
    emit textureHasHorizontalBorderImageSavings((m_horizontalBorderImageSavings > minimumBorderImageSavingsPercent), m_horizontalBorderImageSavings);
    m_horizontalBorderRectMidCut = QRect(leftCol, 0, rightCol - leftCol, textureHeight);
    if (m_horizontalBorderImageSavings > minimumBorderImageSavingsPercent) imageFlags |= ImageFlag::BorderImageCandidate;

    //verticalBorderImage
    auto midRow = textureHeight / 2;
    auto upperRow = midRow;
    breakout = false;
    while ((upperRow > 0) && !breakout) {
        upperRow --;
        for (int col = 0; col < textureWidth; col++) {
            if (analyzedTexture.pixel(col, upperRow) != analyzedTexture.pixel(col, midRow)) {
                breakout = true;
                break;
            }
        }
    }
    auto lowerRow = midRow;
    breakout = false;
    while ((lowerRow < textureHeight - 1) &&! breakout) {
        lowerRow++;
        for (int col = 0; col < textureWidth; col++) {
            if (analyzedTexture.pixel(col, lowerRow) != analyzedTexture.pixel(col, midRow)) {
                breakout = true;
                break;
            }
        }
    }
    m_verticalBorderImageSavings = qRound(((lowerRow - upperRow) * textureWidth) / imagePixelSize * 100);
    emit textureHasVerticalBorderImageSavings((m_verticalBorderImageSavings > minimumBorderImageSavingsPercent), m_verticalBorderImageSavings);
    m_verticalBorderRectMidCut = QRect(0, upperRow, textureWidth, lowerRow - upperRow);
    if (m_verticalBorderImageSavings > minimumBorderImageSavingsPercent) imageFlags |= ImageFlag::BorderImageCandidate;

    // Only hide the Infobar when the texture had no flaws
    emit textureInfoNecessary(imageFlags != ImageFlag::None);
}

