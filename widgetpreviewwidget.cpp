#include "widgetpreviewwidget.h"
#include <QPainter>

using namespace Endoscope;

WidgetPreviewWidget::WidgetPreviewWidget(QWidget* parent): QWidget(parent)
{
}

void WidgetPreviewWidget::setWidget(QWidget* widget)
{
  if ( widget != this )
    m_widget = widget;
  else
    m_widget.clear();
  update();
}

void WidgetPreviewWidget::paintEvent(QPaintEvent* event )
{
  static bool grabbingWidget = false;
  if ( m_widget && !grabbingWidget ) {
    grabbingWidget = true;
    const QPixmap pixmap = QPixmap::grabWidget( m_widget.data() );
    grabbingWidget = false;
    QPainter painter( this );
    const qreal scale = qMin<qreal>( 1.0, qMin( (qreal)width() / (qreal)pixmap.width(), (qreal)height() / (qreal)pixmap.height() ) );
    const qreal targetWidth = pixmap.width() * scale;
    const qreal targetHeight = pixmap.height() * scale;
    painter.drawPixmap( (width() - targetWidth) / 2, (height() - targetHeight) / 2, targetWidth, targetHeight, pixmap );
  }
  QWidget::paintEvent( event );
}

#include "widgetpreviewwidget.moc"
