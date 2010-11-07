#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "objecttypefilterproxymodel.h"
#include "scenemodel.h"
#include "connectionmodel.h"
#include "connectionfilterproxymodel.h"
#include "singlecolumnobjectproxymodel.h"
#include "modelmodel.h"
#include "modelcellmodel.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <qgraphicsscene.h>
#include <qdebug.h>
#include <qgraphicsitem.h>
#include <QtScript/qscriptengine.h>
#include <QtScriptTools/QScriptEngineDebugger>
#include <qwebpage.h>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel( this );
  objectFilter->setSourceModel( Probe::instance()->objectTreeModel() );
  objectFilter->setDynamicSortFilter( true );
  ui.objectTreeView->setModel( objectFilter );
  ui.objectSearchLine->setProxy( objectFilter );
  connect( ui.objectTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	   SLOT(objectSelected(QModelIndex)) );

  ObjectTypeFilterProxyModel<QWidget> *widgetFilterProxy = new ObjectTypeFilterProxyModel<QWidget>( this );
  widgetFilterProxy->setSourceModel( Probe::instance()->objectTreeModel() );
  KRecursiveFilterProxyModel* widgetSearchProxy = new KRecursiveFilterProxyModel( this );
  widgetSearchProxy->setSourceModel( widgetFilterProxy );
  ui.widgetTreeView->setModel( widgetSearchProxy );
  ui.widgetSearchLine->setProxy( widgetSearchProxy );
  connect( ui.widgetTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(widgetSelected(QModelIndex)) );

  KRecursiveFilterProxyModel *modelFilterProxy = new KRecursiveFilterProxyModel( this );
  modelFilterProxy->setSourceModel( Probe::instance()->modelModel() );
  ui.modelView->setModel( modelFilterProxy );
  ui.modelSearchLine->setProxy( modelFilterProxy );
  connect( ui.modelView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(modelSelected(QModelIndex)) );
  m_cellModel = new ModelCellModel( this );
  ui.modelCellView->setModel( m_cellModel );

  ObjectTypeFilterProxyModel<QGraphicsScene> *sceneFilterProxy = new ObjectTypeFilterProxyModel<QGraphicsScene>( this );
  sceneFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  SingleColumnObjectProxyModel* singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( sceneFilterProxy );
  ui.sceneComboBox->setModel( singleColumnProxy );
  connect( ui.sceneComboBox, SIGNAL(activated(int)), SLOT(sceneSelected(int)) );
  m_sceneModel = new SceneModel( this );
  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel( this );
  sceneFilter->setSourceModel( m_sceneModel );
  ui.sceneTreeView->setModel( sceneFilter );
  ui.screneTreeSearchLine->setProxy( sceneFilter );
  connect( ui.sceneTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(sceneItemSelected(QModelIndex)) );

  ObjectTypeFilterProxyModel<QScriptEngine> *scriptEngineFilter = new ObjectTypeFilterProxyModel<QScriptEngine>( this );
  scriptEngineFilter->setSourceModel( Probe::instance()->objectListModel() );
  singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( scriptEngineFilter );
  ui.scriptEngineComboBox->setModel( singleColumnProxy );
  connect( ui.scriptEngineComboBox, SIGNAL(activated(int)), SLOT(scriptEngineSelected(int)) );

  ObjectTypeFilterProxyModel<QWebPage> *webPageFilter = new ObjectTypeFilterProxyModel<QWebPage>( this );
  webPageFilter->setSourceModel( Probe::instance()->objectListModel() );
  singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( webPageFilter );
  ui.webPageComboBox->setModel( singleColumnProxy );
  connect( ui.webPageComboBox, SIGNAL(activated(int)), SLOT(webPageSelected(int)) );

  QSortFilterProxyModel *connectionFilterProxy = new ConnectionFilterProxyModel( this );
  connectionFilterProxy->setSourceModel( Probe::instance()->connectionModel() );
  ui.connectionSearchLine->setProxy( connectionFilterProxy );
  ui.connectionView->setModel( connectionFilterProxy );

  setWindowTitle( tr( "Endoscope (%1)" ).arg( qApp->applicationName() ) );
}

void MainWindow::objectSelected( const QModelIndex &index )
{
  if ( index.isValid() ) {
    QObject *obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    ui.objectPropertyWidget->setObject( obj );
  } else {
    ui.objectPropertyWidget->setObject( 0 );
  }
}

void MainWindow::widgetSelected(const QModelIndex& index)
{
  if ( index.isValid() ) {
    QObject *obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    ui.widgetPropertyWidget->setObject( obj );
    ui.widgetPreviewWidget->setWidget( qobject_cast<QWidget*>( obj ) );
  } else {
    ui.widgetPropertyWidget->setObject( 0 );
    ui.widgetPreviewWidget->setWidget( 0 );
  }
}

void MainWindow::modelSelected( const QModelIndex &index )
{
  if ( index.isValid() ) {
    QObject* obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( obj );
    ui.modelContentView->setModel( model );
    connect( ui.modelContentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
             SLOT(modelCellSelected(QModelIndex)) );
  } else {
    ui.modelContentView->setModel( 0 );
  }
  m_cellModel->setModelIndex( QModelIndex() );
}

void MainWindow::modelCellSelected(const QModelIndex& index)
{
  m_cellModel->setModelIndex( index );
}

void MainWindow::sceneSelected(int index)
{
  QObject* obj = ui.sceneComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QGraphicsScene* scene = qobject_cast<QGraphicsScene*>( obj );
  qDebug() << Q_FUNC_INFO << scene << obj;

  m_sceneModel->setScene( scene );
  ui.graphicsSceneView->setGraphicsScene( scene );
}

void MainWindow::sceneItemSelected(const QModelIndex& index)
{
  if ( index.isValid() ) {
    QGraphicsItem* item = index.data( SceneModel::SceneItemRole ).value<QGraphicsItem*>();
    ui.scenePropertyWidget->setObject( item->toGraphicsObject() );
    ui.graphicsSceneView->showGraphicsItem( item );
  } else {
    ui.scenePropertyWidget->setObject( 0 );
  }
}

void MainWindow::scriptEngineSelected(int index)
{
  QObject* obj = ui.scriptEngineComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QScriptEngine *engine = qobject_cast<QScriptEngine*>( obj );
  if ( engine ) {
    QScriptEngineDebugger *debugger = new QScriptEngineDebugger( this );
    qDebug() << "Attaching debugger" << engine;
    debugger->attachTo( engine );
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    debugger->standardWindow()->show();
  }
}

void MainWindow::webPageSelected(int index)
{
  QObject* obj = ui.webPageComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QWebPage *page = qobject_cast<QWebPage*>( obj );
  if ( page ) {
    page->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
    ui.webInspector->setPage( page );
  }
}

#include "mainwindow.moc"
