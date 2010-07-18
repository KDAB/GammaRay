#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttypefilterproxymodel.h"

#include <KLocalizedString>
#include <QCoreApplication>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );

  QSortFilterProxyModel *objectFilter = new QSortFilterProxyModel( this );
  objectFilter->setSourceModel( Probe::instance()->objectListModel() );
  objectFilter->setDynamicSortFilter( true );
  ui.objectTreeView->setModel( objectFilter );
  ui.objectSearchLine->setProxy( objectFilter );

  ObjectTypeFilterProxyModel<QAbstractItemModel> *modelFilterProxy = new ObjectTypeFilterProxyModel<QAbstractItemModel>( this );
  modelFilterProxy->setSourceModel( Probe::instance()->objectListModel() );
  ui.modelComboBox->setModel( modelFilterProxy );
  connect( ui.modelComboBox, SIGNAL(currentIndexChanged(int)), SLOT(modelSelected(int)) );

  setWindowTitle( i18n( "Endoscope (%1)", qApp->applicationName() ) );
}

void MainWindow::modelSelected( int index )
{
  QObject* obj = ui.modelComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( obj );
  ui.modelContentView->setModel( model );
}


#include "mainwindow.moc"
