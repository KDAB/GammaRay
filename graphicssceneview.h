#ifndef ENDOSCOPE_GRAPHICSSCENEVIEW_H
#define ENDOSCOPE_GRAPHICSSCENEVIEW_H

#include <qwidget.h>

class QGraphicsScene;
class QGraphicsItem;

namespace Endoscope {

namespace Ui {
class GraphicsSceneView;
}

class GraphicsSceneView : public QWidget
{
  Q_OBJECT
  public:
    explicit GraphicsSceneView( QWidget *parent = 0 );
    ~GraphicsSceneView();

    void showGraphicsItem( QGraphicsItem *item );
    void setGraphicsScene( QGraphicsScene *scene );

  private slots:
    void sceneCoordinatesChanged( const QPointF &coord );
    void itemCoordinatesChanged( const QPointF &coord );

  private:
    Ui::GraphicsSceneView* ui;
};

}

#endif // ENDOSCOPE_GRAPHICSSCENEVIEW_H
