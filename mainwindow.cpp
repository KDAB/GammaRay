/*
  mainwindow.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "objecttypefilterproxymodel.h"
#include "toolmodel.h"
#include "toolfactory.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <qdebug.h>
#include <QtGui/QStringListModel>
#include <QtCore/qtextcodec.h>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>

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
    ToolFactory *toolIface = mi.data( ToolModel::ToolFactoryRole ).value<ToolFactory*>();
    Q_ASSERT( toolIface );
    qDebug() << Q_FUNC_INFO << "creating new probe: " << toolIface->name() << toolIface->supportedTypes();
    toolWidget = toolIface->createInstance( Probe::instance(), 0 );
    ui.toolStack->addWidget( toolWidget );
    m_toolSelector->model()->setData( mi, QVariant::fromValue( toolWidget ) );
  }
  ui.toolStack->setCurrentIndex( ui.toolStack->indexOf( toolWidget ) );
}


#include "mainwindow.moc"
