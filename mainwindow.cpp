#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "objecttypefilterproxymodel.h"
#include "objectpropertymodel.h"
#include "scenemodel.h"

#include <KLocalizedString>
#include <QCoreApplication>
#include <qgraphicsscene.h>
#include <qdebug.h>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );

  QSortFilterProxyModel *objectFilter = new QSortFilterProxyModel( this );
  objectFilter->setSourceModel( Probe::instance()->objectTreeModel() );
  objectFilter->setDynamicSortFilter( true );
  ui.objectTreeView->setModel( objectFilter );
  ui.objectSearchLine->setProxy( objectFilter );
  connect( ui.objectTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	   SLOT(objectSelected(QModelIndex)) );

  m_objectPropertyModel = new ObjectPropertyModel( this );
  QSortFilterProxyModel *objectPropertyFilter = new QSortFilterProxyModel( this );
  objectPropertyFilter->setSourceModel( m_objectPropertyModel );
  ui.objectPropertyView->setModel( objectPropertyFilter );
  ui.objectPropertySearchLine->setProxy( objectPropertyFilter );

  ObjectTypeFilterProxyModel<QWidget> *widgetFilterProxy = new ObjectTypeFilterProxyModel<QWidget>( this );
  widgetFilterProxy->setSourceModel( Probe::instance()->objectTreeModel() );
  ui.widgetTreeView->setModel( widgetFilterProxy );
  ui.widgetSearchLine->setProxy( widgetFilterProxy );

  ObjectTypeFilterProxyModel<QAbstractItemModel> *modelFilterProxy = new ObjectTypeFilterProxyModel<QAbstractItemModel>( this );
  modelFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  ui.modelComboBox->setModel( modelFilterProxy );
  connect( ui.modelComboBox, SIGNAL(currentIndexChanged(int)), SLOT(modelSelected(int)) );

  ObjectTypeFilterProxyModel<QGraphicsScene> *sceneFilterProxy = new ObjectTypeFilterProxyModel<QGraphicsScene>( this );
  sceneFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  ui.sceneComboBox->setModel( sceneFilterProxy );
  connect( ui.sceneComboBox, SIGNAL(activated(int)), SLOT(sceneSelected(int)) );
  m_sceneModel = new SceneModel( this );
  QSortFilterProxyModel *sceneFilter = new QSortFilterProxyModel( this );
  sceneFilter->setSourceModel( m_sceneModel );
  ui.sceneTreeView->setModel( sceneFilter );
  ui.screneTreeSearchLine->setProxy( sceneFilter );

  setWindowTitle( i18n( "Endoscope (%1)", qApp->applicationName() ) );
}

void MainWindow::objectSelected( const QModelIndex &index )
{
  if ( index.isValid() ) {
    QObject *obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    m_objectPropertyModel->setObject( obj );
  } else {
    m_objectPropertyModel->setObject( 0 );
  }
}

void MainWindow::modelSelected( int index )
{
  QObject* obj = ui.modelComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( obj );
  ui.modelContentView->setModel( model );
}

void MainWindow::sceneSelected(int index)
{
  QObject* obj = ui.sceneComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QGraphicsScene* scene = qobject_cast<QGraphicsScene*>( obj );
  qDebug() << Q_FUNC_INFO << scene << obj;

  m_sceneModel->setScene( scene );
}

#include "mainwindow.moc"
