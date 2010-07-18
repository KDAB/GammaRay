#include "scenemodel.h"

#include <qgraphicsitem.h>
#include <qgraphicsscene.h>

using namespace Endoscope;

SceneModel::SceneModel(QObject* parent):
  QAbstractItemModel(parent),
  m_scene( 0 )
{
}

void SceneModel::setScene(QGraphicsScene* scene)
{
  m_scene = scene;
  reset();
}

QVariant SceneModel::data(const QModelIndex& index, int role) const
{
  QGraphicsItem *item = 0;
  if ( !index.isValid() )
    return QVariant();
  if ( !index.parent().isValid() && index.row() >= 0 && index.row() < m_scene->items().size() )
    item = m_scene->items().at( index.row() );

  if ( item && role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return QLatin1String( "0x" ) + QString::number( reinterpret_cast<qlonglong>( item ), 16 );
    else if ( index.column() == 1 )
      return QString::number( item->type() );
  }
  return QVariant();
}

int SceneModel::columnCount(const QModelIndex& parent) const
{
  return 2;
}

int SceneModel::rowCount(const QModelIndex& parent) const
{
  if ( !m_scene )
    return 0;
  if ( parent.isValid() ) {
    QGraphicsItem* item = static_cast<QGraphicsItem*>( parent.internalPointer() );
    if ( item )
      return item->childItems().size();
    else
      return 0;
  }
  return m_scene->items().size();
}

QModelIndex SceneModel::parent(const QModelIndex& child) const
{
  if ( !child.isValid() )
    return QModelIndex();
  return QModelIndex();
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex& parent) const
{
  return createIndex( row, column, 0 );
}

#include "scenemodel.moc"
