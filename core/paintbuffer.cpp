/*
  paintbuffer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintbuffer.h"
#include "execution.h"

#ifdef HAVE_PRIVATE_QT_HEADERS

using namespace GammaRay;

#ifdef USE_GAMMARAY_PAINTBUFFER
PaintBufferEngine::PaintBufferEngine(GammaRay::PaintBuffer *buffer)
    : QPaintBufferEngine(buffer->data())
    , m_buffer(buffer)
{
}

PaintBufferEngine::~PaintBufferEngine()
{
}

void PaintBufferEngine::clip(const QVectorPath& path, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(path, op);
    createStackTrace();
}

void PaintBufferEngine::clip(const QRect& rect, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(rect, op);
    createStackTrace();
}

void PaintBufferEngine::clip(const QRegion& region, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(region, op);
    createStackTrace();
}

void PaintBufferEngine::clip(const QPainterPath& path, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(path, op);
    // TODO disabled in QPaintBuffer
//     createStackTrace();
}

void PaintBufferEngine::backgroundModeChanged()
{
    QPaintBufferEngine::backgroundModeChanged();
    createStackTrace();
}

void PaintBufferEngine::brushChanged()
{
    QPaintBufferEngine::brushChanged();
    createStackTrace();
}

void PaintBufferEngine::brushOriginChanged()
{
    QPaintBufferEngine::brushOriginChanged();
    createStackTrace();
}

void PaintBufferEngine::clipEnabledChanged()
{
    QPaintBufferEngine::clipEnabledChanged();
    createStackTrace();
}

void PaintBufferEngine::compositionModeChanged()
{
    QPaintBufferEngine::compositionModeChanged();
    createStackTrace();
}

void PaintBufferEngine::opacityChanged()
{
    QPaintBufferEngine::opacityChanged();
    createStackTrace();
}

void PaintBufferEngine::penChanged()
{
    QPaintBufferEngine::penChanged();
    createStackTrace();
}

void PaintBufferEngine::renderHintsChanged()
{
    QPaintBufferEngine::renderHintsChanged();
    createStackTrace();
}

void PaintBufferEngine::transformChanged()
{
    QPaintBufferEngine::transformChanged();
    createStackTrace();
}

void PaintBufferEngine::fillRect(const QRectF& rect, const QBrush& brush)
{
    QPaintBufferEngine::fillRect(rect, brush);
    createStackTrace();
}

void PaintBufferEngine::fillRect(const QRectF& rect, const QColor& color)
{
    QPaintBufferEngine::fillRect(rect, color);
    createStackTrace();
}

void PaintBufferEngine::drawRects(const QRect* rects, int rectCount)
{
    QPaintBufferEngine::drawRects(rects, rectCount);
    createStackTrace();
}

void PaintBufferEngine::drawRects(const QRectF* rects, int rectCount)
{
    QPaintBufferEngine::drawRects(rects, rectCount);
    createStackTrace();
}

void PaintBufferEngine::drawLines(const QLine* lines, int lineCount)
{
    QPaintBufferEngine::drawLines(lines, lineCount);
    createStackTrace();
}

void PaintBufferEngine::drawLines(const QLineF* lines, int lineCount)
{
    QPaintBufferEngine::drawLines(lines, lineCount);
    createStackTrace();
}

void PaintBufferEngine::drawEllipse(const QRect& r)
{
    QPaintBufferEngine::drawEllipse(r);
    createStackTrace();
}

void PaintBufferEngine::drawEllipse(const QRectF& r)
{
    QPaintBufferEngine::drawEllipse(r);
    createStackTrace();
}

void PaintBufferEngine::drawPath(const QPainterPath& path)
{
    QPaintBufferEngine::drawPath(path);
    // TODO this is not handled in QPaintBuffer
//     createStackTrace();
}

void PaintBufferEngine::drawPoints(const QPoint* points, int pointCount)
{
    QPaintBufferEngine::drawPoints(points, pointCount);
    createStackTrace();
}

void PaintBufferEngine::drawPoints(const QPointF* points, int pointCount)
{
    QPaintBufferEngine::drawPoints(points, pointCount);
    createStackTrace();
}

void PaintBufferEngine::drawPolygon(const QPoint* points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    QPaintBufferEngine::drawPolygon(points, pointCount, mode);
    createStackTrace();
}

void PaintBufferEngine::drawPolygon(const QPointF* points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    QPaintBufferEngine::drawPolygon(points, pointCount, mode);
    createStackTrace();
}

void PaintBufferEngine::drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr)
{
    QPaintBufferEngine::drawPixmap(r, pm, sr);
    createStackTrace();
}

void PaintBufferEngine::drawPixmap(const QPointF& pos, const QPixmap& pm)
{
    QPaintBufferEngine::drawPixmap(pos, pm);
    createStackTrace();
}

void PaintBufferEngine::drawImage(const QPointF& pos, const QImage& image)
{
    QPaintBufferEngine::drawImage(pos, image);
    createStackTrace();
}

void PaintBufferEngine::drawImage(const QRectF& r, const QImage& pm, const QRectF& sr, Qt::ImageConversionFlags flags)
{
    QPaintBufferEngine::drawImage(r, pm, sr, flags);
    createStackTrace();
}

void PaintBufferEngine::drawTiledPixmap(const QRectF& r, const QPixmap& pixmap, const QPointF& s)
{
    QPaintBufferEngine::drawTiledPixmap(r, pixmap, s);
    createStackTrace();
}

void PaintBufferEngine::drawTextItem(const QPointF& pos, const QTextItem& ti)
{
    QPaintBufferEngine::drawTextItem(pos, ti);
    createStackTrace();
}

void PaintBufferEngine::drawStaticTextItem(QStaticTextItem* staticTextItem)
{
    QPaintBufferEngine::drawStaticTextItem(staticTextItem);
    createStackTrace();
}

void PaintBufferEngine::setState(QPainterState* s)
{
    const auto prevSize = m_buffer->data()->commands.size();
    QPaintBufferEngine::setState(s);
    const auto newSize = m_buffer->data()->commands.size();
    if (prevSize != newSize)
        createStackTrace();
}

void PaintBufferEngine::createStackTrace()
{
    if (!Execution::stackTracingAvailable())
        return;

    const auto size = m_buffer->data()->commands.size();
    m_buffer->m_stackTraces.resize(size);
    // TODO find a way to stop this at the analyzer call site, we don't want to see the gammaray call chain
    m_buffer->m_stackTraces.back() = Execution::stackTrace(16, 2);
}
#endif


class PaintBufferPrivacyViolater : public QPainterReplayer
{
public:
    static QPaintBufferPrivate* get(QPaintBuffer *buffer)
    {
        PaintBufferPrivacyViolater p;
        p.processCommands(*buffer, nullptr, 0, -1); // end < begin -> no processing
        return p.extract();
    }
private:
    QPaintBufferPrivate *extract() const { return d; }
};


PaintBuffer::PaintBuffer()
{
    d = PaintBufferPrivacyViolater::get(this);
}

PaintBuffer::PaintBuffer(const PaintBuffer& other)
    : QPaintBuffer(other)
    , m_stackTraces(other.m_stackTraces)
{
    d = PaintBufferPrivacyViolater::get(this);
}

PaintBuffer::~PaintBuffer()
{
}

PaintBuffer& PaintBuffer::operator=(const PaintBuffer& other)
{
    QPaintBuffer::operator=(other);
    d = PaintBufferPrivacyViolater::get(this);
    m_stackTraces = other.m_stackTraces;
    return *this;
}

QPaintEngine* PaintBuffer::paintEngine() const
{
#ifdef USE_GAMMARAY_PAINTBUFFER
    if (!d->engine)
        d->engine = new PaintBufferEngine(const_cast<PaintBuffer*>(this));
    return d->engine;
#else
    return QPaintBuffer::paintEngine();
#endif
}

Execution::Trace PaintBuffer::stackTrace(int index) const
{
    if (index < 0 || index >= m_stackTraces.size())
        return Execution::Trace();
    return m_stackTraces.at(index);
}

QPaintBufferPrivate* PaintBuffer::data() const
{
    return d;
}

#endif
