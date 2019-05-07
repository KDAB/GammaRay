/*
  textureviewwidget.cpp

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

#include "textureviewwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <cmath>

using namespace GammaRay;

TextureViewWidget::TextureViewWidget(QWidget *parent)
    : RemoteViewWidget(parent)
    , m_visualizeTextureProblems(true)
    , m_pixelWasteInPercent(0)
    , m_pixelWasteInBytes(0)
    , m_horizontalBorderImageSavingsInPercent(0)
    , m_verticalBorderImageSavings(0)
{
    connect(this, &RemoteViewWidget::frameChanged, this, &TextureViewWidget::analyzeImageFlaws);
}

TextureViewWidget::~TextureViewWidget() = default;

void TextureViewWidget::drawPixelWasteDecoration(QPainter *p) const
{
    //Draw Warning if more than 30% or 1KB are wasted
    const auto hasTextureWasteProblem =
        (m_pixelWasteInPercent > transparencyWasteLimitInPercent ||
         m_pixelWasteInBytes > transparencyWasteLimitInBytes);
    if (!hasTextureWasteProblem)
        return;

    p->save();
    auto scaleTransform = QTransform::fromScale(zoom(), zoom());
    p->setTransform(scaleTransform, true);

    //Draw Wasted Area
    QPen pen(Qt::red);
    pen.setCosmetic(true);
    p->setPen(pen);
    QBrush brush = QBrush(Qt::red, Qt::FDiagPattern);
    brush.setTransform(scaleTransform.inverted());
    p->setBrush(brush);
    auto outerRect = m_analyzedRect;
    auto wasteArea = QPainterPath();
    wasteArea.addRect(outerRect);
    auto innerRect = QPainterPath();
    auto translatedBoundingRect = m_opaqueBoundingRect.translated(m_analyzedRect.topLeft());
    innerRect.addRect(translatedBoundingRect);
    wasteArea = wasteArea.subtracted(innerRect);
    p->drawPath(wasteArea);
    p->restore();
}

void TextureViewWidget::drawBorderImageCutouts(QPainter *p) const
{
    p->save();
    auto scaleTransform = QTransform::fromScale(zoom(), zoom());
    p->setTransform(scaleTransform, true);

    QPen pen(Qt::white);
    pen.setCosmetic(true);
    p->setPen(pen);
    QBrush brush = QBrush(Qt::white, Qt::FDiagPattern);
    brush.setTransform(scaleTransform.inverted());
    p->setBrush(brush);
    if (m_horizontalBorderImageSavingsInPercent > minimumBorderImageSavingsPercent) {
        auto horizontalCutout = m_horizontalBorderRectMidCut.translated(m_analyzedRect.topLeft());
        p->drawRect(horizontalCutout);
    }
    if (m_verticalBorderImageSavings > minimumBorderImageSavingsPercent) {
        auto verticalCutout = m_verticalBorderRectMidCut.translated(m_analyzedRect.topLeft());
        p->drawRect(verticalCutout);
    }
    p->restore();
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
    if (m_visualizeTextureProblems){
        drawBorderImageCutouts(p);
        drawPixelWasteDecoration(p);
    }

    drawActiveAtlasTile(p);
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
    int atlasTextureOffset = 0; // atlas textures are 1 pixel bigger
    auto atlasSubTile = frame().data().toRect();
    if (atlasSubTile.isValid()) { //Atlas-Case
        analyzedTexture = frame().image().copy(atlasSubTile);
        analyzedRect = atlasSubTile;
        analyzedRect = analyzedRect.adjusted(-1, -1, 1, 1);
        atlasTextureOffset = 1;
    } else { // Whole-Texture-Case
        analyzedTexture = frame().image();
        analyzedRect = frame().image().rect();
        atlasTextureOffset = 0;
    }

    m_analyzedRect = analyzedRect;

    QRgb possibleSingularColor = analyzedTexture.pixel(0, 0);
    ImageFlags imageFlags = FullyTransparent | Unicolor;
    int top = analyzedTexture.height(), bottom = 0, left = analyzedTexture.width(), right = 0;

    for (int y = 0; y < analyzedTexture.height(); y++) {
        for (int x = 0; x < analyzedTexture.width(); x++) {
            auto pixel = analyzedTexture.pixel(x, y);

            if (Q_UNLIKELY(imageFlags.testFlag(Unicolor)) && (possibleSingularColor != pixel))
                imageFlags &=~ Unicolor;

            if (qAlpha(pixel) != 0) {
                imageFlags &=~ FullyTransparent;
                top = std::min(top, y);
                bottom = std::max(bottom, y);
                left = std::min(left, x);
                right = std::max(right, x);
            }
        }
    }

    m_opaqueBoundingRect = QRect(QPoint(left, top), QPoint(right, bottom)).translated(QPoint(atlasTextureOffset, atlasTextureOffset));

    // Calculate Waste
    const float imagePixelSize = (analyzedTexture.width() * analyzedTexture.height());
    const auto pixelWaste = 1.0 - ((m_opaqueBoundingRect.height() * m_opaqueBoundingRect.width()) / imagePixelSize);
    m_pixelWasteInPercent = qRound(pixelWaste * 100.0f);
    m_pixelWasteInBytes = (imagePixelSize - (m_opaqueBoundingRect.height() * m_opaqueBoundingRect.width())) * frame().image().depth() / 8;

    // Emit all possible Problems so far
    auto hasTextureWasteProblem = (m_pixelWasteInPercent > transparencyWasteLimitInPercent || m_pixelWasteInBytes > transparencyWasteLimitInBytes);
    emit textureWasteFound(hasTextureWasteProblem, m_pixelWasteInPercent, m_pixelWasteInBytes);
    if (hasTextureWasteProblem) imageFlags |= TextureWaste;
    emit textureIsUnicolor(imageFlags.testFlag(Unicolor));
    emit textureIsFullyTransparent(imageFlags.testFlag(FullyTransparent));

    // Border Image checks
    // horizontal mid cut
    auto textureWidth = analyzedTexture.width();
    auto textureHeight = analyzedTexture.height();
    auto midCol = textureWidth / 2;
    auto breakout = false;
    int leftCol;
    for (leftCol = midCol; (leftCol >= 0) && !breakout; leftCol--) {
        for (int row = 0; row < textureHeight - 1; row++) {
            if (analyzedTexture.pixel(leftCol, row) != analyzedTexture.pixel(midCol, row)) {
                leftCol += 2;
                breakout = true;
                break;
            }
        }
    }
    breakout = false;
    int rightCol;
    for (rightCol = midCol; (rightCol < textureWidth) && !breakout; rightCol++) {
        for (int row = 0; row < textureHeight; row++) {
            if (analyzedTexture.pixel(rightCol, row) != analyzedTexture.pixel(midCol, row)) {
                rightCol -= 2;
                breakout = true;
                break;
            }
        }
    }
    m_horizontalBorderImageSavingsInPercent = qRound(((rightCol - leftCol + 1) * textureHeight) / imagePixelSize * 100);
    m_horizontalBorderRectMidCut = QRect(leftCol + atlasTextureOffset, 0, rightCol - leftCol + 1, analyzedRect.height());
    // vertical mid cut
    auto midRow = textureHeight / 2;
    int upperRow;
    breakout = false;
    for (upperRow = midRow; (upperRow >= 0) && !breakout; upperRow--) {
        for (int col = 0; col < textureWidth; col++) {
            if (analyzedTexture.pixel(col, upperRow) != analyzedTexture.pixel(col, midRow)) {
                upperRow += 2;
                breakout = true;
                break;
            }
        }
    }
    breakout = false;
    int lowerRow;
    for (lowerRow = midRow; (lowerRow < textureHeight - 1) && !breakout; lowerRow++) {
        for (int col = 0; col < textureWidth; col++) {
            if (analyzedTexture.pixel(col, lowerRow) != analyzedTexture.pixel(col, midRow)) {
                lowerRow -= 2;
                breakout = true;
                break;
            }
        }
    }
    m_verticalBorderImageSavings = qRound(((lowerRow - upperRow + 1) * textureWidth) / imagePixelSize * 100);
    m_verticalBorderRectMidCut = QRect(0, upperRow + atlasTextureOffset, analyzedRect.width(), lowerRow - upperRow + 1);

    auto overallSavingsInPercent = 0;
    auto area = [](const QRect &r){ return r.width() * r.height(); };
    const auto hs = (m_horizontalBorderImageSavingsInPercent > minimumBorderImageSavingsPercent);
    const auto vs = (m_verticalBorderImageSavings > minimumBorderImageSavingsPercent);
    if (!hs && !vs) overallSavingsInPercent = 0;
    if ( hs && !vs) overallSavingsInPercent = m_horizontalBorderImageSavingsInPercent;
    if (!hs &&  vs) overallSavingsInPercent = m_verticalBorderImageSavings;
    if ( hs &&  vs) {
        const auto overlapRect = m_horizontalBorderRectMidCut.intersected(m_verticalBorderRectMidCut);
        overallSavingsInPercent = area(m_horizontalBorderRectMidCut) + area(m_verticalBorderRectMidCut) - area(overlapRect);
        overallSavingsInPercent = qRound(overallSavingsInPercent / ((float) area(m_analyzedRect)) * 100);
    }
    if (overallSavingsInPercent > minimumBorderImageSavingsPercent) imageFlags |= BorderImageCandidate;
    auto overallSavingsInBytes = overallSavingsInPercent / 100.0f * area(m_analyzedRect) * frame().image().depth() / 8;
    emit textureHasBorderImageSavings((overallSavingsInPercent > minimumBorderImageSavingsPercent), overallSavingsInPercent, overallSavingsInBytes);

    // Show or hide the infobar depending on found issues
    emit textureInfoNecessary(imageFlags != None);
}
