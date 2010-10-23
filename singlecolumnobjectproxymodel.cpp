#include "singlecolumnobjectproxymodel.h"
#include "util.h"
#include "objectlistmodel.h"

using namespace Endoscope;

SingleColumnObjectProxyModel::SingleColumnObjectProxyModel(QObject* parent) : KIdentityProxyModel(parent)
{
}

QVariant SingleColumnObjectProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
  if ( proxyIndex.isValid() && role == Qt::DisplayRole && proxyIndex.column() == 0 ) {
    const QObject *obj = proxyIndex.data( ObjectListModel::ObjectRole ).value<QObject*>();
    if ( obj )
      return Util::displayString( obj );
  }
  
  return KIdentityProxyModel::data(proxyIndex, role);
}

#include "singlecolumnobjectproxymodel.moc"
