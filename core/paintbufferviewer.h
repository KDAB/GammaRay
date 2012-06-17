/*
  paintbufferviewer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PAINTBUFFERVIEWER_H
#define GAMMARAY_PAINTBUFFERVIEWER_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS

#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase

#include <QWidget>

namespace GammaRay {

class PaintBufferModel;

namespace Ui {
  class PaintBufferViewer;
}

/**
 * A widget to look at the command list in a QPaintBuffer.
 */
class PaintBufferViewer : public QWidget
{
  Q_OBJECT
  public:
    explicit PaintBufferViewer(QWidget *parent = 0);
    virtual ~PaintBufferViewer();

    void setPaintBuffer(const QPaintBuffer &buffer);

  private slots:
    void commandSelected();
    void zoomChanged(int value);

  private:
    QScopedPointer<Ui::PaintBufferViewer> ui;
    QPaintBuffer m_buffer;
    PaintBufferModel *m_bufferModel;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERVIEWER_H
