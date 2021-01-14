/*
  paintbuffer.cpp

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

#include "paintbuffer.h"
#include "execution.h"


using namespace GammaRay;

PaintBufferEngine::PaintBufferEngine(GammaRay::PaintBuffer *buffer)
    : QPaintBufferEngine(buffer->data())
    , m_buffer(buffer)
{
}

PaintBufferEngine::~PaintBufferEngine() = default;

void PaintBufferEngine::clip(const QVectorPath& path, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(path, op);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::clip(const QRect& rect, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(rect, op);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::clip(const QRegion& region, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(region, op);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::clip(const QPainterPath& path, Qt::ClipOperation op)
{
    QPaintBufferEngine::clip(path, op);
    // TODO disabled in QPaintBuffer
//     createStackTrace();
//      pushOrigin();
}

void PaintBufferEngine::backgroundModeChanged()
{
    QPaintBufferEngine::backgroundModeChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::brushChanged()
{
    QPaintBufferEngine::brushChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::brushOriginChanged()
{
    QPaintBufferEngine::brushOriginChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::clipEnabledChanged()
{
    QPaintBufferEngine::clipEnabledChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::compositionModeChanged()
{
    QPaintBufferEngine::compositionModeChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::opacityChanged()
{
    QPaintBufferEngine::opacityChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::penChanged()
{
    QPaintBufferEngine::penChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::renderHintsChanged()
{
    QPaintBufferEngine::renderHintsChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::transformChanged()
{
    QPaintBufferEngine::transformChanged();
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::fillRect(const QRectF& rect, const QBrush& brush)
{
    QPaintBufferEngine::fillRect(rect, brush);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::fillRect(const QRectF& rect, const QColor& color)
{
    QPaintBufferEngine::fillRect(rect, color);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawRects(const QRect* rects, int rectCount)
{
    QPaintBufferEngine::drawRects(rects, rectCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawRects(const QRectF* rects, int rectCount)
{
    QPaintBufferEngine::drawRects(rects, rectCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawLines(const QLine* lines, int lineCount)
{
    QPaintBufferEngine::drawLines(lines, lineCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawLines(const QLineF* lines, int lineCount)
{
    QPaintBufferEngine::drawLines(lines, lineCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawEllipse(const QRect& r)
{
    QPaintBufferEngine::drawEllipse(r);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawEllipse(const QRectF& r)
{
    QPaintBufferEngine::drawEllipse(r);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPath(const QPainterPath& path)
{
    QPaintBufferEngine::drawPath(path);
    // TODO this is not handled in QPaintBuffer
//     createStackTrace();
//      pushOrigin();
}

void PaintBufferEngine::drawPoints(const QPoint* points, int pointCount)
{
    QPaintBufferEngine::drawPoints(points, pointCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPoints(const QPointF* points, int pointCount)
{
    QPaintBufferEngine::drawPoints(points, pointCount);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPolygon(const QPoint* points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    QPaintBufferEngine::drawPolygon(points, pointCount, mode);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPolygon(const QPointF* points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    QPaintBufferEngine::drawPolygon(points, pointCount, mode);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr)
{
    QPaintBufferEngine::drawPixmap(r, pm, sr);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawPixmap(const QPointF& pos, const QPixmap& pm)
{
    QPaintBufferEngine::drawPixmap(pos, pm);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawImage(const QPointF& pos, const QImage& image)
{
    QPaintBufferEngine::drawImage(pos, image);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawImage(const QRectF& r, const QImage& pm, const QRectF& sr, Qt::ImageConversionFlags flags)
{
    QPaintBufferEngine::drawImage(r, pm, sr, flags);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawTiledPixmap(const QRectF& r, const QPixmap& pixmap, const QPointF& s)
{
    QPaintBufferEngine::drawTiledPixmap(r, pixmap, s);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawTextItem(const QPointF& pos, const QTextItem& ti)
{
    QPaintBufferEngine::drawTextItem(pos, ti);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::drawStaticTextItem(QStaticTextItem* staticTextItem)
{
    QPaintBufferEngine::drawStaticTextItem(staticTextItem);
    createStackTrace();
    pushOrigin();
}

void PaintBufferEngine::setState(QPainterState* s)
{
    const auto prevSize = m_buffer->data()->commands.size();
    QPaintBufferEngine::setState(s);
    const auto newSize = m_buffer->data()->commands.size();
    if (prevSize != newSize) {
        createStackTrace();
        pushOrigin();
    }
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
void PaintBufferEngine::pushOrigin()
{
    const auto size = m_buffer->data()->commands.size();
    m_buffer->m_origins.reserve(size);
    while (m_buffer->m_origins.size() < size) {
        m_buffer->m_origins.push_back(m_buffer->m_currentOrigin);
    }
}


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
    , m_origins(other.m_origins)
{
    d = PaintBufferPrivacyViolater::get(this);
}

PaintBuffer::~PaintBuffer() = default;

PaintBuffer& PaintBuffer::operator=(const PaintBuffer& other)
{
    QPaintBuffer::operator=(other);
    d = PaintBufferPrivacyViolater::get(this);
    m_stackTraces = other.m_stackTraces;
    m_origins = other.m_origins;
    return *this;
}

QPaintEngine* PaintBuffer::paintEngine() const
{
    if (!d->engine)
        d->engine = new PaintBufferEngine(const_cast<PaintBuffer*>(this));
    return d->engine;
}

Execution::Trace PaintBuffer::stackTrace(int index) const
{
    if (index < 0 || index >= m_stackTraces.size())
        return Execution::Trace();
    return m_stackTraces.at(index);
}

ObjectId PaintBuffer::origin(int index) const
{
    if (index < 0 || index >= m_origins.size()) {
        return ObjectId();
    }
    return m_origins.at(index);
}

void PaintBuffer::setOrigin(const ObjectId &obj)
{
    m_currentOrigin = obj;
}

QPaintBufferPrivate* PaintBuffer::data() const
{
    return d;
}
