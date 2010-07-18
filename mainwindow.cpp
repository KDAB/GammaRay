#include "mainwindow.h"

#include <KLocalizedString>
#include <QCoreApplication>

using namespace Endoscope;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
  QWidget *center = new QWidget( this );
  setCentralWidget( center );
  ui.setupUi( centralWidget() );
  setWindowTitle( i18n( "Endoscope (%1)", qApp->applicationName() ) );
}


#include "mainwindow.moc"
