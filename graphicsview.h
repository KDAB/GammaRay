#ifndef ENDOSCOPE_GRAPHICSVIEW_H
#define ENDOSCOPE_GRAPHICSVIEW_H

#include <QGraphicsView>

namespace Endoscope {

class GraphicsView : public QGraphicsView
{
  Q_OBJECT
  public:
    explicit GraphicsView( QWidget* parent = 0 );

    void showItem( QGraphicsItem* item );

  signals:
    void sceneCoordinatesChanged( const QPointF &sceneCoord );
    void itemCoordinatesChanged( const QPointF &itemCoord );

  protected:
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void drawForeground(QPainter* painter, const QRectF& rect);

  private:
    QGraphicsItem *m_currentItem;
};

}

#endif // ENDOSCOPE_GRAPHICSVIEW_H
