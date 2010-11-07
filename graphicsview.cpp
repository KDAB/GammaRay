#include "graphicsview.h"

#include <QKeyEvent>
#include <qgraphicsitem.h>

using namespace Endoscope;

GraphicsView::GraphicsView(QWidget* parent): QGraphicsView(parent),
  m_currentItem( 0 )
{
  setMouseTracking( true );
}

void GraphicsView::showItem(QGraphicsItem* item)
{
  m_currentItem = item;
  fitInView( item, Qt::KeepAspectRatio );
  scale( 0.8f, 0.8f );
}

void GraphicsView::keyPressEvent(QKeyEvent* event)
{
  if ( event->modifiers() == Qt::CTRL ) {
    switch ( event->key() ) {
      case Qt::Key_Plus:
        scale( 1.2, 1.2 );
        event->accept();
        return;
      case Qt::Key_Minus:
        scale( 0.8, 0.8 );
        event->accept();
        return;
      case Qt::Key_Left:
        rotate( -5 );
        event->accept();
        break;
      case Qt::Key_Right:
        rotate( 5 );
        event->accept();
        break;
    }
  }
  QGraphicsView::keyPressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
  emit sceneCoordinatesChanged( mapToScene( event->pos() ) );
  if ( m_currentItem )
    emit itemCoordinatesChanged( m_currentItem->mapFromScene( mapToScene( event->pos() ) ) );
  QGraphicsView::mouseMoveEvent(event);
}


void GraphicsView::drawForeground(QPainter* painter, const QRectF& rect)
{
  QGraphicsView::drawForeground(painter, rect);
  if ( m_currentItem ) {
    const QRectF itemBoundingRect = m_currentItem->boundingRect();
    // coord system, TODO: nicer axis with arrows, tics, markers for current mouse position etc.
    painter->setPen( Qt::black );
    const qreal maxX = qMax( qAbs(itemBoundingRect.left()), qAbs(itemBoundingRect.right()) );
    const qreal maxY = qMax( qAbs(itemBoundingRect.top()), qAbs(itemBoundingRect.bottom()) );
    const qreal maxXY = qMax( maxX, maxY ) * 1.5f;
    painter->drawLine( m_currentItem->mapToScene( -maxXY, 0 ), m_currentItem->mapToScene( maxXY, 0 ) );
    painter->drawLine( m_currentItem->mapToScene( 0, -maxXY ), m_currentItem->mapToScene( 0, maxXY ) );
    
    painter->setPen( Qt::blue );
    const QPolygonF boundingBox = m_currentItem->mapToScene( itemBoundingRect );
    painter->drawPolygon( boundingBox );

    painter->setPen( Qt::green );
    const QPainterPath shape = m_currentItem->mapToScene( m_currentItem->shape() );
    painter->drawPath( shape );

    painter->setPen( Qt::red );
    const QPointF transformOrigin = m_currentItem->mapToScene( m_currentItem->transformOriginPoint() );
    painter->drawEllipse( transformOrigin, 5.0/transform().m11(), 5.0/transform().m22() );
  }
}



#include "graphicsview.moc"
