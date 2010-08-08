#include "objectenummodel.h"
#include <qmetaobject.h>

using namespace Endoscope;

Endoscope::ObjectEnumModel::ObjectEnumModel(QObject* parent): MetaObjectModel<QMetaEnum, &QMetaObject::enumerator, &QMetaObject::enumeratorCount, &QMetaObject::enumeratorOffset>( parent )
{
}

int Endoscope::ObjectEnumModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 2;
}

QVariant ObjectEnumModel::data(const QModelIndex& index, const QMetaEnum& enumerator, int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return QString::fromLatin1( enumerator.name() );
  }
  return QVariant();
}

QString ObjectEnumModel::columnHeader(int index) const
{
  switch ( index ) {
    case 0: return i18n( "Name" );
  }
  return QString();
}
