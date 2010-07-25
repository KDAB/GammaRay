#include "objectdynamicpropertymodel.h"

using namespace Endoscope;

ObjectDynamicPropertyModel::ObjectDynamicPropertyModel(QObject* parent):
  ObjectPropertyModel(parent)
{
}

QVariant ObjectDynamicPropertyModel::data(const QModelIndex& index, int role) const
{
  if ( !m_obj )
    return QVariant();
  
  const QList<QByteArray> propNames = m_obj.data()->dynamicPropertyNames();
  if ( index.row() < 0 || index.row() >= propNames.size() )
    return QVariant();

  const QByteArray propName = propNames.at( index.row() );

  if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
    const QVariant propValue = m_obj.data()->property( propName );
    if ( index.column() == 0 )
      return QString::fromUtf8( propName );
    else if ( index.column() == 1 )
      return propValue;
    else if ( index.column() == 2 )
      return propValue.typeName();
  }

  return QVariant();
}

int ObjectDynamicPropertyModel::columnCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return 3;
}

int ObjectDynamicPropertyModel::rowCount(const QModelIndex& parent) const
{
  if ( !m_obj || parent.isValid() )
    return 0;
  return m_obj.data()->dynamicPropertyNames().size();
}

#include "objectdynamicpropertymodel.moc"
