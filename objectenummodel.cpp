#include "objectenummodel.h"
#include <qmetaobject.h>

using namespace Endoscope;

typedef MetaObjectModel<QMetaEnum, &QMetaObject::enumerator, &QMetaObject::enumeratorCount, &QMetaObject::enumeratorOffset> SuperClass;

Endoscope::ObjectEnumModel::ObjectEnumModel(QObject* parent): SuperClass( parent )
{
}

int ObjectEnumModel::rowCount(const QModelIndex& parent) const
{
  if ( !parent.isValid() )
    return SuperClass::rowCount(parent);
  if ( parent.parent().isValid() )
    return 0;
  const QMetaEnum e = m_object.data()->metaObject()->enumerator( parent.row() );
  return e.keyCount();
}

int Endoscope::ObjectEnumModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 3;
}

QVariant ObjectEnumModel::data(const QModelIndex& index, int role) const
{
  if ( !index.parent().isValid() )
    return SuperClass::data(index, role);

  if ( role == Qt::DisplayRole ) {
    const QMetaEnum e = m_object.data()->metaObject()->enumerator( index.parent().row() );
    if ( index.column() == 0 )
      return e.key( index.row() );
    if ( index.column() == 1 )
      return e.value( index.row() );
  }

  return QVariant();
}

QVariant ObjectEnumModel::data(const QModelIndex& index, const QMetaEnum& enumerator, int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return QString::fromLatin1( enumerator.name() );
    if ( index.column() == 1 )
      return i18np( "1 element", "%1 elements", enumerator.keyCount() );
  }
  return QVariant();
}

QString ObjectEnumModel::columnHeader(int index) const
{
  switch ( index ) {
    case 0: return i18n( "Name" );
    case 1: return i18n( "Value" );
  }
  return QString();
}

QModelIndex Endoscope::ObjectEnumModel::index(int row, int column, const QModelIndex& parent) const
{
  if ( !parent.isValid() )
    return SuperClass::index(row, column, parent);
  return createIndex( row, column, parent.row() );
}

QModelIndex Endoscope::ObjectEnumModel::parent(const QModelIndex& child) const
{
  if ( child.internalId() == -1 )
    return SuperClass::parent( child );
  return SuperClass::index( child.internalId(), 0, QModelIndex() );
}
