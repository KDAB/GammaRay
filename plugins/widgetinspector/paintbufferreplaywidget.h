/*
  paintbufferreplaywidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_PAINTBUFFERREPLAYWIDGET_H
#define GAMMARAY_PAINTBUFFERREPLAYWIDGET_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <QWidget>

#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase

namespace GammaRay {

/**
 * A widget painting a certain sub-set of a QPaintBuffer.
 */
class PaintBufferReplayWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit PaintBufferReplayWidget(QWidget *parent = 0);

    void setPaintBuffer(const QPaintBuffer &buffer);
    void setEndCommandIndex(int index);
    void setZoomFactor(int zoom);

    virtual QSize sizeHint() const;

  protected:
    virtual void paintEvent(QPaintEvent *event);

  private:
    static void drawTransparencyPattern(QPainter *painter, const QRect &rect, int squareSize = 16);

  private:
    QPaintBuffer m_buffer;
    int m_endCommandIndex;
    int m_zoomFactor;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERREPLAYWIDGET_H
