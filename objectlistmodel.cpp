#include "objectlistmodel.h"

using namespace Endoscope;

ObjectListModel::ObjectListModel(QObject* parent): QAbstractTableModel(parent)
{
}

QVariant ObjectListModel::data(const QModelIndex& index, int role) const
{
  if ( index.row() >= 0 && index.row() < m_objects.size() ) {
    QObject *obj = m_objects.at( index.row() );
    if ( role == Qt::DisplayRole ) {
      if ( index.column() == 0 )
	return obj->objectName().isEmpty() ? QString::number( reinterpret_cast<qlonglong>( obj ), 16 ) : obj->objectName();
      else if ( index.column() == 1 )
	return obj->metaObject()->className();
    }
  }
  return QVariant();
}

int ObjectListModel::columnCount(const QModelIndex& parent) const
{
  return 2;
}

int ObjectListModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return m_objects.size();
}

void Endoscope::ObjectListModel::objectAdded(QObject* obj)
{
  m_objects.push_back( obj );
}

void Endoscope::ObjectListModel::objectRemoved(QObject* obj)
{
  const int index = m_objects.indexOf( obj );
  if ( index < 0 || index >= m_objects.size() )
    return;
  beginRemoveRows( QModelIndex(), index, index );
  m_objects.remove( index );
  endRemoveRows();
}

#include "objectlistmodel.moc"
