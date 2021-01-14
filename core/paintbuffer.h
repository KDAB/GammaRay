/*
  paintbuffer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PAINTBUFFER_H
#define GAMMARAY_PAINTBUFFER_H

#include <config-gammaray.h>
#include <common/objectid.h>
#include <QVector>

#include <private/qpaintbuffer_p.h>

namespace GammaRay {
namespace Execution {
class Trace;
}

class PaintBuffer;

class PaintBufferEngine : public QPaintBufferEngine
{
public:
    explicit PaintBufferEngine(GammaRay::PaintBuffer *buffer);
    ~PaintBufferEngine();

    void clip(const QVectorPath &path, Qt::ClipOperation op) override;
    void clip(const QRect &rect, Qt::ClipOperation op) override;
    void clip(const QRegion &region, Qt::ClipOperation op) override;
    void clip(const QPainterPath &path, Qt::ClipOperation op) override;

    void backgroundModeChanged() override;
    void brushChanged() override;
    void brushOriginChanged() override;
    void clipEnabledChanged() override;
    void compositionModeChanged() override;
    void opacityChanged() override;
    void penChanged() override;
    void renderHintsChanged() override;
    void transformChanged() override;

    void fillRect(const QRectF &rect, const QBrush &brush) override;
    void fillRect(const QRectF &rect, const QColor &color) override;

    void drawRects(const QRect *rects, int rectCount) override;
    void drawRects(const QRectF *rects, int rectCount) override;

    void drawLines(const QLine *lines, int lineCount) override;
    void drawLines(const QLineF *lines, int lineCount) override;

    void drawEllipse(const QRectF &r) override;
    void drawEllipse(const QRect &r) override;

    void drawPath(const QPainterPath &path) override;

    void drawPoints(const QPoint *points, int pointCount) override;
    void drawPoints(const QPointF *points, int pointCount) override;

    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode) override;
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) override;

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) override;
    void drawPixmap(const QPointF &pos, const QPixmap &pm) override;

    void drawImage(const QPointF &pos, const QImage &image) override;
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor) override;

    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s) override;

    void drawTextItem(const QPointF &pos, const QTextItem &ti) override;
    void drawStaticTextItem(QStaticTextItem *staticTextItem) override;

    void setState(QPainterState *s) override;

private:
    void createStackTrace();
    void pushOrigin();

    GammaRay::PaintBuffer *m_buffer;
};

class PaintBuffer : public QPaintBuffer
{
public:
    PaintBuffer();
    PaintBuffer(const PaintBuffer &other);
    ~PaintBuffer();
    PaintBuffer& operator=(const PaintBuffer &other);

    QPaintEngine *paintEngine() const override;

    /**
     * Marks all following paint operations to origin from the given QWidget/QQuickItem
     * until this is called with another object.
     */
    void setOrigin(const ObjectId &obj);

    /** Returns the stack trace of command at @p index. */
    Execution::Trace stackTrace(int index) const;

    /** Returns the origin of command at @p index. */
    ObjectId origin(int index) const;



    QPaintBufferPrivate* data() const;
private:
    friend class PaintBufferEngine;
    QPaintBufferPrivate *d; // not protected in the base class, somewhat nasty to get to
    QVector<Execution::Trace> m_stackTraces;
public:
    QVector<ObjectId> m_origins;
    ObjectId m_currentOrigin;
};

}

#endif // GAMMARAY_PAINTBUFFER_H
