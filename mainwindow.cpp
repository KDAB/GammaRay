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
#include "statemodel.h"
#include "transitionmodel.h"
#include "metatypesmodel.h"
#include "fontmodel.h"
#include "codecmodel.h"
#include "toolmodel.h"
#include "toolinterface.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <qgraphicsscene.h>
#include <qdebug.h>
#include <qgraphicsitem.h>

#include <qt/resourcemodel.h>
#include <QtGui/QItemSelection>
#include <QtCore/QStateMachine>
#include <QFontDatabase>
#include <QtGui/QStringListModel>
#include <QtCore/qtextcodec.h>
#include <QtGui/QMessageBox>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  ui.setupUi( this );

  connect( ui.actionRetractProbe, SIGNAL(triggered(bool)), SLOT(close()) );
  connect( QApplication::instance(), SIGNAL(aboutToQuit()), SLOT(close()) );
  connect( ui.actionQuit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()) );
  connect( ui.actionAboutQt, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(aboutQt()) );
  connect( ui.actionAboutEndoscope, SIGNAL(triggered(bool)), SLOT(about()) );

  connect( Probe::instance(), SIGNAL(widgetSelected(QWidget*)), SLOT(widgetSelected(QWidget*)) );
  connect( Probe::instance(), SIGNAL(graphicsItemSelected(QGraphicsItem*)), SLOT(sceneItemSelected(QGraphicsItem*)) );

  setWindowIcon( QIcon(":endoscope/endoscope128.png") );

  m_toolSelector = new QComboBox;
  m_toolSelector->setModel( Probe::instance()->toolModel() );
  connect( m_toolSelector, SIGNAL(currentIndexChanged(int)), SLOT(toolSelected()) );
  connect( m_toolSelector, SIGNAL(activated(int)), SLOT(toolSelected()) );
  ui.mainToolBar->addWidget( new QLabel( tr("Select Probe:") ) );
  ui.mainToolBar->addWidget( m_toolSelector );

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

  QSortFilterProxyModel *connectionFilterProxy = new ConnectionFilterProxyModel( this );
  connectionFilterProxy->setSourceModel( Probe::instance()->connectionModel() );
  ui.connectionSearchLine->setProxy( connectionFilterProxy );
  ui.connectionView->setModel( connectionFilterProxy );

  ResourceModel *resourceModel = new ResourceModel(this);
  ui.treeView->setModel(resourceModel);
  ui.treeView->expandAll();
  connect( ui.treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(resourceSelected(QItemSelection,QItemSelection)));

  ObjectTypeFilterProxyModel<QStateMachine> *stateMachineFilter = new ObjectTypeFilterProxyModel<QStateMachine>( this );
  stateMachineFilter->setSourceModel( Probe::instance()->objectListModel() );
  ui.stateMachinesView->setModel(stateMachineFilter);
  connect( ui.stateMachinesView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(stateMachineSelected(QItemSelection,QItemSelection)));
  m_stateModel = 0;

  m_transitionModel = new TransitionModel(this);
  ui.transitionView->setModel(m_transitionModel);

  MetaTypesModel *mtm = new MetaTypesModel(this);

  ui.metaTypeView->setModel(mtm);

  ObjectTypeFilterProxyModel<QItemSelectionModel> *selectionModelProxy = new ObjectTypeFilterProxyModel<QItemSelectionModel>( this );
  selectionModelProxy->setSourceModel( Probe::instance()->objectListModel() );
  ui.selectionModelView->setModel(selectionModelProxy);
  connect(ui.selectionModelView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(selectionModelSelected(QItemSelection,QItemSelection)));

  setWindowTitle( tr( "Endoscope (%1)" ).arg( qApp->applicationName() ) );

  QFontDatabase database;

  m_selectedFontModel = new FontModel(this);
  ui.selectedFontsView->setModel(m_selectedFontModel);

  ui.fontTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  foreach (const QString &family, database.families()) {
      QTreeWidgetItem *familyItem = new QTreeWidgetItem(ui.fontTree);
      familyItem->setText(0, family);

      foreach (const QString &style, database.styles(family)) {
          QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
          styleItem->setText(0, style);

          QString sizes;
          foreach (int points, database.smoothSizes(family, style))
              sizes += QString::number(points) + ' ';

          styleItem->setText(1, sizes.trimmed());
      }
  }
  connect(ui.fontTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateFonts(QItemSelection,QItemSelection)));
  connect(ui.fontText, SIGNAL(textChanged(QString)), m_selectedFontModel, SLOT(updateText(QString)));

  ui.codecList->setModel(new AllCodecsModel(this));
  ui.codecList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_selectedCodecsModel = new SelectedCodecsModel(this);
  ui.selectedCodecs->setModel(m_selectedCodecsModel);

  connect(ui.codecList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateCodecs(QItemSelection,QItemSelection)));
  connect(ui.codecText, SIGNAL(textChanged(QString)), m_selectedCodecsModel, SLOT(updateText(QString)));
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

void MainWindow::widgetSelected(QWidget* widget)
{
  QAbstractItemModel *model = ui.widgetTreeView->model();
  const QModelIndexList indexList = model->match( model->index( 0, 0 ), ObjectTreeModel::ObjectRole, QVariant::fromValue<QObject*>( widget ), 1, Qt::MatchExactly | Qt::MatchRecursive );
  if ( indexList.isEmpty() )
    return;
  const QModelIndex index = indexList.first();
  ui.widgetTreeView->selectionModel()->select( index, QItemSelectionModel::Select | QItemSelectionModel::Clear | QItemSelectionModel::Rows | QItemSelectionModel::Current );
  ui.widgetTreeView->scrollTo( index );
  widgetSelected( index );
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

void MainWindow::sceneItemSelected(QGraphicsItem* item)
{
  QAbstractItemModel *model = ui.sceneTreeView->model();
  const QModelIndexList indexList = model->match( model->index( 0, 0 ), SceneModel::SceneItemRole, QVariant::fromValue<QGraphicsItem*>( item ), 1, Qt::MatchExactly | Qt::MatchRecursive );
  if ( indexList.isEmpty() )
    return;
  const QModelIndex index = indexList.first();
  ui.sceneTreeView->selectionModel()->select( index, QItemSelectionModel::Select | QItemSelectionModel::Clear | QItemSelectionModel::Rows | QItemSelectionModel::Current );
  ui.sceneTreeView->scrollTo( index );
  sceneItemSelected( index );
}

void MainWindow::resourceSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
  Q_UNUSED(deselected)
  const QModelIndex selectedRow = selected.first().topLeft();
  const QFileInfo fi(selectedRow.data( ResourceModel::FilePathRole ).toString());

  if ( fi.isFile() ) {
    const QStringList l = QStringList() << "jpg" << "png" << "jpeg";
    if ( l.contains( fi.suffix() ) ) {
      ui.label_3->setPixmap( fi.absoluteFilePath() );
      ui.stackedWidget->setCurrentWidget(ui.page_4);
    } else {
      QFile f( fi.absoluteFilePath() );
      f.open(QFile::ReadOnly | QFile::Text);
      ui.textBrowser->setText( f.readAll() );
      ui.stackedWidget->setCurrentWidget(ui.page_3);
    }
  }
}

void MainWindow::stateMachineSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
  Q_UNUSED(deselected)
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *machineObject = selectedRow.data( ObjectListModel::ObjectRole ).value<QObject*>();
  QStateMachine *machine = qobject_cast<QStateMachine*>(machineObject);
  if (machine) {
    delete m_stateModel;
    m_stateModel = new StateModel( machine, this );
    ui.singleStateMachineView->setModel(m_stateModel);
    ui.singleStateMachineView->expandAll();
    connect(ui.singleStateMachineView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                                                         SLOT(stateSelected(QItemSelection,QItemSelection)));
  }
}

void MainWindow::stateSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *stateObject = selectedRow.data( StateModel::StateObjectRole ).value<QObject*>();
  QState *state = qobject_cast<QState*>(stateObject);
  if (state) {
    m_transitionModel->setState(state);
  }
}

void MainWindow::selectionModelSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *selectionModelObject = selectedRow.data( ObjectListModel::ObjectRole ).value<QObject*>();
  QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel*>(selectionModelObject);
  if (selectionModel && selectionModel->model()) {
    ui.selectionModelVisualizer->setModel(const_cast<QAbstractItemModel*>(selectionModel->model()));
    ui.selectionModelVisualizer->setSelectionModel(selectionModel);
  }

}

void MainWindow::updateFonts(const QItemSelection& selected, const QItemSelection& deselected)
{
  QList<QFont> previousFonts = m_selectedFontModel->currentFonts();
  QStringList previousFontNames;
  foreach(const QFont &f, previousFonts) {
    previousFontNames.append(f.family());
  }
  QList<QFont> currentFonts;
  QStringList currentFontNames;
  foreach(const QModelIndex &index, ui.fontTree->selectionModel()->selectedRows()) {
    if (index.parent().isValid())
      continue;
    QFont font(index.data().toString());
    currentFontNames.append(font.family());
    if (previousFontNames.contains(font.family())) {
      continue;
    }
    currentFonts.append(font);
  }
  {
    QList<QFont>::iterator it = previousFonts.begin();
    while ( it != previousFonts.end()) {
      if (!currentFontNames.contains(it->family()))
        it = previousFonts.erase(it);
      else
        ++it;
    }
  }

  currentFonts << previousFonts;
  m_selectedFontModel->updateFonts(currentFonts);
}

void MainWindow::updateCodecs(const QItemSelection& selected, const QItemSelection& deselected)
{
  QStringList previousCodecs = m_selectedCodecsModel->currentCodecs();

  QStringList currentCodecNames;
  foreach(const QModelIndex &index, ui.codecList->selectionModel()->selectedRows()) {
    const QString codecName = index.data().toString();
    if (previousCodecs.contains(codecName)) {
      continue;
    }
    currentCodecNames.append(codecName);
  }

  currentCodecNames << previousCodecs;
  m_selectedCodecsModel->setCodecs(currentCodecNames);
}

void MainWindow::about()
{
  QMessageBox mb( this );
  mb.setText( tr("<b>Endoscope 1.0</b>") );
  mb.setInformativeText( tr("<qt>(C)&nbsp;Copyright&nbsp;2010,&nbsp;2011,&nbsp;KDAB</qt>" ) );
  mb.setIconPixmap( QPixmap( ":endoscope/endoscope128.png" ) );
  mb.addButton( QMessageBox::Close );
  mb.exec();
}

void MainWindow::toolSelected()
{
  const QModelIndex mi = m_toolSelector->model()->index( m_toolSelector->currentIndex(), 0 );
  QWidget *toolWidget = mi.data( ToolModel::ToolWidgetRole ).value<QWidget*>();
  if ( !toolWidget ) {
    ToolInterface *toolIface = mi.data( ToolModel::ToolInterfaceRole ).value<ToolInterface*>();
    Q_ASSERT( toolIface );
    qDebug() << Q_FUNC_INFO << "creating new probe: " << toolIface->name() << toolIface->supportedTypes();
    toolWidget = toolIface->createInstance( Probe::instance(), 0 );
    ui.toolStack->addWidget( toolWidget );
    m_toolSelector->model()->setData( mi, QVariant::fromValue( toolWidget ) );
  }
  ui.toolStack->setCurrentIndex( ui.toolStack->indexOf( toolWidget ) );
}


#include "mainwindow.moc"
