/*
  textureviewwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TEXTUREVIEWWIDGET_H
#define GAMMARAY_TEXTUREVIEWWIDGET_H

#include <ui/remoteviewwidget.h>


namespace GammaRay {

class TextureViewWidget : public RemoteViewWidget
{
    Q_OBJECT
public:
    explicit TextureViewWidget(QWidget *parent = nullptr);
    ~TextureViewWidget() override;

    enum ImageFlag
    {
        None = 0,
        FullyTransparent = 1,
        Unicolor = 2,
        TextureWaste = 4,
        BorderImageCandidate = 8
    };
    Q_DECLARE_FLAGS(ImageFlags, ImageFlag)

    void drawDecoration(QPainter *p) override;
    void drawPixelWasteDecoration(QPainter *p) const;
    void drawBorderImageCutouts(QPainter *p) const;

    void drawActiveAtlasTile(QPainter *p) const;

    const static int transparencyWasteLimitInPercent = 30;
    const static int transparencyWasteLimitInBytes = 16 * 1024;
    const static int minimumBorderImageSavingsPercent = 25;

signals:
    void textureInfoNecessary(bool isNecessary);
    void textureWasteFound(bool, int percent, int bytes);
    void textureIsUnicolor(bool uniColor);
    void textureIsFullyTransparent(bool fullyTransparent);
    void textureHasBorderImageSavings(bool isBorderImage, int percent, int bytes);

public slots:
    void setTextureWasteVisualizationEnabled(bool enabled);

private slots:
    void analyzeImageFlaws();

private:
    bool m_visualizeTextureProblems;
    int m_pixelWasteInPercent;
    int m_pixelWasteInBytes;
    QRect m_analyzedRect;
    QRect m_opaqueBoundingRect; // area actually occupied by opaque pixels
    int m_horizontalBorderImageSavingsInPercent;
    QRect m_horizontalBorderRectMidCut;
    int m_verticalBorderImageSavings;
    QRect m_verticalBorderRectMidCut;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::TextureViewWidget::ImageFlags)

#endif // GAMMARAY_TEXTUREVIEWWIDGET_H
