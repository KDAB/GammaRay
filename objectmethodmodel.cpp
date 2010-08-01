#include "objectmethodmodel.h"

#include <KLocalizedString>

using namespace Endoscope;

ObjectMethodModel::ObjectMethodModel(QObject* parent) :
  MetaObjectModel<QMetaMethod, &QMetaObject::method, &QMetaObject::methodCount, &QMetaObject::methodOffset>(parent)
{
}

int Endoscope::ObjectMethodModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 4;
}

QVariant ObjectMethodModel::data(const QModelIndex& index, const QMetaMethod& method, int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return method.signature();
    if ( index.column() == 1 ) {
      switch ( method.methodType() ) {
        case QMetaMethod::Method: return i18n( "Method" );
        case QMetaMethod::Constructor: return i18n( "Constructor" );
        case QMetaMethod::Slot: return i18n( "Slot" );
        case QMetaMethod::Signal: return i18n( "Signal" );
        default: return i18n( "Unknown" );
      }
    }
    if ( index.column() == 2 ) {
      switch ( method.access() ) {
        case QMetaMethod::Public: return i18n( "Public" );
        case QMetaMethod::Protected: return i18n( "Protected" );
        case QMetaMethod::Private: return i18n( "Private" );
        default: return i18n( "Unknown" );
      }
    }
  } else if ( role == MetaMethodRole ) {
    return QVariant::fromValue( method );
  }
  return QVariant();
}

QString Endoscope::ObjectMethodModel::columnHeader(int index) const
{
  switch ( index ) {
    case 0: return i18n( "Signature" );
    case 1: return i18n( "Type" );
    case 2: return i18n( "Access" );
  }
  return QString();
}
