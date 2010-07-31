#include "widgetpreviewwidget.h"

#include <QEvent>
#include <QPainter>

using namespace Endoscope;

WidgetPreviewWidget::WidgetPreviewWidget(QWidget* parent): 
  QWidget(parent),
  m_grabbingWidget( false )
{
}

void WidgetPreviewWidget::setWidget(QWidget* widget)
{
  if ( m_widget )
    m_widget.data()->removeEventFilter( this );
  if ( widget != this )
    m_widget = widget;
  else
    m_widget.clear();
  if ( widget )
    widget->installEventFilter( this );
  update();
}

void WidgetPreviewWidget::paintEvent(QPaintEvent* event )
{
  if ( m_widget && !m_grabbingWidget ) {
    m_grabbingWidget = true;
    const QPixmap pixmap = QPixmap::grabWidget( m_widget.data() );
    m_grabbingWidget = false;
    QPainter painter( this );
    const qreal scale = qMin<qreal>( 1.0, qMin( (qreal)width() / (qreal)pixmap.width(), (qreal)height() / (qreal)pixmap.height() ) );
    const qreal targetWidth = pixmap.width() * scale;
    const qreal targetHeight = pixmap.height() * scale;
    painter.drawPixmap( (width() - targetWidth) / 2, (height() - targetHeight) / 2, targetWidth, targetHeight, pixmap );
  }
  QWidget::paintEvent( event );
}

bool WidgetPreviewWidget::eventFilter( QObject *receiver, QEvent *event )
{
  if ( !m_grabbingWidget && receiver == m_widget.data() && event->type() == QEvent::Paint ) {
    update();
  }
  return QWidget::eventFilter( receiver, event );
}

#include "widgetpreviewwidget.moc"
