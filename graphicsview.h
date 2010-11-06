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

  protected:
    void keyPressEvent(QKeyEvent* event);
};

}

#endif // ENDOSCOPE_GRAPHICSVIEW_H
