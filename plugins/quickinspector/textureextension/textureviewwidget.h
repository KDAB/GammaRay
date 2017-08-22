/*
  textureviewwidget.h

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

#ifndef GAMMARAY_TEXTUREVIEWWIDGET_H
#define GAMMARAY_TEXTUREVIEWWIDGET_H

#include <ui/remoteviewwidget.h>


namespace GammaRay {

class TextureViewWidget : public RemoteViewWidget
{
    Q_OBJECT
public:
    explicit TextureViewWidget(QWidget *parent = nullptr);
    ~TextureViewWidget();

    void drawDecoration(QPainter *p) override;
    void drawPixelWasteDecoration(QPainter *p) const;
    void drawActiveAtlasTile(QPainter *p) const;

    const static int transparencyWasteLimitInPercent = 30;
    const static int transparencyWasteLimitInBytes = 1024;

signals:
    void textureInfoNecessary(const bool isNecessary) const;
    void textureWasteFound(const int percent, const int bytes) const;

private slots:
    void setTextureWasteVisualizationEnabled(bool enabled);
    void recalculateBoundingRect();

private:

    bool m_visualizeTextureWaste;
    int m_pixelWasteInPercent;
    int m_pixelWasteInBytes;
    QRect m_analyzedRect;
    QRect m_opaqueBoundingRect; //area actually occupied
};

}

#endif // GAMMARAY_TEXTUREVIEWWIDGET_H
