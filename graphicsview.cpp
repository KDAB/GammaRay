#include "graphicsview.h"

#include <QKeyEvent>

using namespace Endoscope;

GraphicsView::GraphicsView(QWidget* parent): QGraphicsView(parent)
{
  setDragMode( ScrollHandDrag );
}

void GraphicsView::showItem(QGraphicsItem* item)
{
  fitInView( item, Qt::KeepAspectRatio );
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


#include "graphicsview.moc"
