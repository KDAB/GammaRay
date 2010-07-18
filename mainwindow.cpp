#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"

#include <KLocalizedString>
#include <QCoreApplication>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );
  ui.objectTreeView->setModel( Probe::instance()->objectListModel() );
  setWindowTitle( i18n( "Endoscope (%1)", qApp->applicationName() ) );
}


#include "mainwindow.moc"
