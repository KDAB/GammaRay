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
