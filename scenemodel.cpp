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
  if ( !index.isValid() )
    return QVariant();
  QGraphicsItem *item = static_cast<QGraphicsItem*>( index.internalPointer() );

  if ( item && role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return QLatin1String( "0x" ) + QString::number( reinterpret_cast<qlonglong>( item ), 16 );
    else if ( index.column() == 1 ) {
      QGraphicsObject *obj = item->toGraphicsObject();
      if ( obj )
	return obj->metaObject()->className();
      return QString::number( item->type() );
    }
  }
  return QVariant();
}

int SceneModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
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
  QGraphicsItem* item = static_cast<QGraphicsItem*>( child.internalPointer() );
  if ( !item->parentItem() )
    return QModelIndex();
  int row = item->parentItem()->childItems().indexOf( item );
  return createIndex( row, 0, item->parentItem() );
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex& parent) const
{
  if ( !parent.isValid() && row >= 0 && row < m_scene->items().size() )
    return createIndex( row, column, m_scene->items().at( row ) );
  QGraphicsItem* parentItem = static_cast<QGraphicsItem*>( parent.internalPointer() );
  if ( !parentItem || parent.row() < 0 || parent.row() >= parentItem->childItems().size() )
    return QModelIndex();
  return createIndex( row, column, parentItem->childItems().at( row ) );
}

#include "scenemodel.moc"
