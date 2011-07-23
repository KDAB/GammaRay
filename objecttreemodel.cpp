#include "objecttreemodel.h"

#include <QtCore/QEvent>

using namespace Endoscope;

ObjectTreeModel::ObjectTreeModel(QObject* parent): ObjectModelBase< QAbstractItemModel >(parent)
{
}

void ObjectTreeModel::objectAdded(QObject *obj)
{
  if ( m_childParentMap.contains( obj ) )
    return;
  QVector<QObject*> &children = m_parentChildMap[ obj->parent() ];
  const QModelIndex index = indexForObject( obj->parent() );
  if ( index.isValid() || !obj->parent() )
    beginInsertRows( index, children.size(), children.size() );
  children.push_back( obj );
  m_childParentMap.insert( obj, obj->parent() );
  if ( index.isValid() || !obj->parent() )
    endInsertRows();
}

void ObjectTreeModel::objectRemoved(QObject* obj)
{
  if ( !m_childParentMap.contains( obj ) )
    return;
  QObject *parentObj = m_childParentMap[ obj ];
  const QModelIndex parentIndex = indexForObject( parentObj );
  if ( parentObj && !parentIndex.isValid() )
    return;
  QVector<QObject*> &children = m_parentChildMap[ parentObj ];
  const int index = children.indexOf( obj );
  if ( index < 0 || index >= children.size() )
    return;
  beginRemoveRows( parentIndex, index, index );
  children.remove( index );
  m_childParentMap.remove( obj );
  if ( m_parentChildMap.value( obj ).isEmpty() )
    m_parentChildMap.remove( obj );
  endRemoveRows();
}

QVariant ObjectTreeModel::data(const QModelIndex& index, int role) const
{
  QObject *obj = reinterpret_cast<QObject*>( index.internalPointer() );
  if ( obj )
    return dataForObject( obj, index, role );
  return QVariant();
}

int ObjectTreeModel::rowCount(const QModelIndex& parent) const
{
  QObject *parentObj = reinterpret_cast<QObject*>( parent.internalPointer() );
  return m_parentChildMap.value( parentObj ).size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex& child) const
{
  QObject *childObj = reinterpret_cast<QObject*>( child.internalPointer() );
  return indexForObject( m_childParentMap.value( childObj ) );
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  QObject *parentObj = reinterpret_cast<QObject*>( parent.internalPointer() );
  const QVector<QObject*> children = m_parentChildMap.value( parentObj );
  if ( row < 0 || column < 0 || row >= children.size()  || column >= columnCount() )
    return QModelIndex();
  return createIndex( row, column, children.at( row ) );
}

QModelIndex ObjectTreeModel::indexForObject( QObject* object ) const
{
  if ( !object )
    return QModelIndex();
  QObject *parent = m_childParentMap.value( object );
  const QModelIndex parentIndex = indexForObject( parent );
  if ( !parentIndex.isValid() && parent )
    return QModelIndex();
  int row = m_parentChildMap[ parent ].indexOf( object );
  if ( row < 0 )
    return QModelIndex();
  return index( row, 0, parentIndex );
}

#include "objecttreemodel.moc"

