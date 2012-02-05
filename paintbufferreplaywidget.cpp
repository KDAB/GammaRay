#include "paintbufferreplaywidget.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
using namespace GammaRay;

PaintBufferReplayWidget::PaintBufferReplayWidget(QWidget* parent): QWidget(parent), m_endCommandIndex(0)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void PaintBufferReplayWidget::setPaintBuffer(const QPaintBuffer& buffer)
{
  m_buffer = buffer;
  update();
}

void PaintBufferReplayWidget::setEndCommandIndex(int index)
{
  m_endCommandIndex = index;
  update();
}

QSize PaintBufferReplayWidget::sizeHint() const
{
  return m_buffer.boundingRect().size().toSize();
}

void PaintBufferReplayWidget::paintEvent(QPaintEvent* event)
{
  // didn't manage painting on the widget directly, even with the correct translation it is always clipping as if the widget was at 0,0 of its parent
  QImage img(sizeHint(), QImage::Format_ARGB32);
  img.fill(Qt::white);
  QPainter imgPainter(&img);
  int depth = m_buffer.processCommands(&imgPainter, m_buffer.frameStartIndex(0), m_buffer.frameStartIndex(0) + m_endCommandIndex);
  for (;depth > 0;--depth)
    imgPainter.restore();
  imgPainter.end();

  QPainter p(this);
  p.drawImage(QPoint(0,0), img);
}

#include "paintbufferreplaywidget.moc"
#endif
