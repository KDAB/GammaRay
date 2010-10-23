#include "scenemodel.h"

#include <KLocalizedString>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qdebug.h>

#include <boost/bind.hpp>
#include <algorithm>

using namespace Endoscope;

SceneModel::SceneModel(QObject* parent):
  QAbstractItemModel(parent),
  m_scene( 0 )
{
}

void SceneModel::setScene(QGraphicsScene* scene)
{
  m_scene = scene;
  qDebug() << "total amount of graphics items:" << m_scene->items().size();
  reset();
}

QVariant SceneModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() )
    return QVariant();
  QGraphicsItem *item = static_cast<QGraphicsItem*>( index.internalPointer() );

  if ( item && role == Qt::DisplayRole ) {
    QGraphicsObject *obj = item->toGraphicsObject();
    if ( index.column() == 0 ) {
      if ( obj && !obj->objectName().isEmpty() )
	return obj->objectName();
      return QLatin1String( "0x" ) + QString::number( reinterpret_cast<qlonglong>( item ), 16 );
    } else if ( index.column() == 1 ) {
      if ( obj )
	return obj->metaObject()->className();
      return QString::number( item->type() );
    }
  } else if ( role == SceneItemRole ) {
    return QVariant::fromValue( item );
  } else if ( item && role == Qt::ForegroundRole ) {
    if ( !item->isVisible() )
      return Qt::gray;
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
  return topLevelItems().size();
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
  if ( !parent.isValid() && row >= 0 && row < topLevelItems().size() )
    return createIndex( row, column, topLevelItems().at( row ) );
  QGraphicsItem* parentItem = static_cast<QGraphicsItem*>( parent.internalPointer() );
  if ( !parentItem || row < 0 || row >= parentItem->childItems().size() )
    return QModelIndex();
  return createIndex( row, column, parentItem->childItems().at( row ) );
}

QList<QGraphicsItem*> SceneModel::topLevelItems() const
{
  QList<QGraphicsItem*> topLevel;
  if ( !m_scene )
    return topLevel;
  const QList<QGraphicsItem*> allItems = m_scene->items();
  std::remove_copy_if( allItems.begin(), allItems.end(), std::back_inserter( topLevel ),
		       boost::bind( &QGraphicsItem::parentItem, _1 ) != (QGraphicsItem*)(0) );
  return topLevel;
}

QVariant SceneModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
    switch ( section ) {
      case 0: return i18n( "Item" );
      case 1: return i18n( "Type" );
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


#include "scenemodel.moc"
