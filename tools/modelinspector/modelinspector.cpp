/*
  modelinspector.cpp

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

#include "modelinspector.h"
#include "ui_modelinspector.h"

#include "modelmodel.h"
#include "modelcellmodel.h"

#include <kde/krecursivefilterproxymodel.h>
#include <objectlistmodel.h>
#include <probeinterface.h>

using namespace Endoscope;

ModelInspector::ModelInspector(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::ModelInspector )
{
  ui->setupUi( this );

  KRecursiveFilterProxyModel *modelFilterProxy = new KRecursiveFilterProxyModel( this );
  modelFilterProxy->setSourceModel( probe->modelModel() );
  ui->modelView->setModel( modelFilterProxy );
  ui->modelSearchLine->setProxy( modelFilterProxy );
  connect( ui->modelView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
           SLOT(modelSelected(QModelIndex)) );
  m_cellModel = new ModelCellModel( this );
  ui->modelCellView->setModel( m_cellModel );
}

void ModelInspector::modelSelected(const QModelIndex& index)
{
  if ( index.isValid() ) {
    QObject* obj = index.data( ObjectListModel::ObjectRole ).value<QObject*>();
    QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( obj );
    ui->modelContentView->setModel( model );
    connect( ui->modelContentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
             SLOT(modelCellSelected(QModelIndex)) );
  } else {
    ui->modelContentView->setModel( 0 );
  }
  m_cellModel->setModelIndex( QModelIndex() );
}

void ModelInspector::modelCellSelected(const QModelIndex& index)
{
  m_cellModel->setModelIndex( index );
}

#include "modelinspector.moc"
