#include "objectmethodmodel.h"

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
        case QMetaMethod::Method: return tr( "Method" );
        case QMetaMethod::Constructor: return tr( "Constructor" );
        case QMetaMethod::Slot: return tr( "Slot" );
        case QMetaMethod::Signal: return tr( "Signal" );
        default: return tr( "Unknown" );
      }
    }
    if ( index.column() == 2 ) {
      switch ( method.access() ) {
        case QMetaMethod::Public: return tr( "Public" );
        case QMetaMethod::Protected: return tr( "Protected" );
        case QMetaMethod::Private: return tr( "Private" );
        default: return tr( "Unknown" );
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
    case 0: return tr( "Signature" );
    case 1: return tr( "Type" );
    case 2: return tr( "Access" );
  }
  return QString();
}
