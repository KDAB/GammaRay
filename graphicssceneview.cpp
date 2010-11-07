#include "graphicssceneview.h"
#include "ui_graphicssceneview.h"

using namespace Endoscope;

GraphicsSceneView::GraphicsSceneView(QWidget* parent) : QWidget(parent),
  ui( new Ui::GraphicsSceneView )
{
  ui->setupUi( this );

  QFontMetrics fm( ui->sceneCoordLabel->font() );
  ui->sceneCoordLabel->setFixedWidth( fm.width( "00000.00 x 00000.00" ) );
  ui->itemCoordLabel->setFixedWidth( fm.width( "00000.00 x 00000.00" ) );
  
  connect( ui->graphicsView, SIGNAL(sceneCoordinatesChanged(QPointF)), SLOT(sceneCoordinatesChanged(QPointF)) );
  connect( ui->graphicsView, SIGNAL(itemCoordinatesChanged(QPointF)), SLOT(itemCoordinatesChanged(QPointF)) );
}

GraphicsSceneView::~GraphicsSceneView()
{
  delete ui;
}

void GraphicsSceneView::showGraphicsItem(QGraphicsItem* item)
{
  ui->graphicsView->showItem( item );
}

void GraphicsSceneView::setGraphicsScene(QGraphicsScene* scene)
{
  ui->graphicsView->setScene( scene );
}

void GraphicsSceneView::sceneCoordinatesChanged(const QPointF& coord)
{
  ui->sceneCoordLabel->setText( QString::fromLatin1( "%1 x %2" ).arg( coord.x(), 0, 'f', 2 ).arg( coord.y(), 0, 'f', 2 ) );
}

void GraphicsSceneView::itemCoordinatesChanged(const QPointF& coord)
{
  ui->itemCoordLabel->setText( QString::fromLatin1( "%1 x %2" ).arg( coord.x(), 0, 'f', 2 ).arg( coord.y(), 0, 'f', 2 ) );
}

#include "graphicssceneview.moc"
