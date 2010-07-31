#include "objectclassinfomodel.h"

#include <KLocalizedString>

using namespace Endoscope;

ObjectClassInfoModel::ObjectClassInfoModel(QObject* parent) :
  MetaObjectModel<QMetaClassInfo, &QMetaObject::classInfo, &QMetaObject::classInfoCount, &QMetaObject::classInfoOffset>( parent )
{
}

QVariant ObjectClassInfoModel::data(const QModelIndex& index, const QMetaClassInfo& classInfo, int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return classInfo.name();
    if ( index.column() == 1 )
      return classInfo.value();
  }
  return QVariant();
}

int ObjectClassInfoModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 3;
}

QString Endoscope::ObjectClassInfoModel::columnHeader(int index) const
{
  switch ( index ) {
    case 0: return i18n( "Name" );
    case 1: return i18n( "Value" );
  }
  return QString();
}
