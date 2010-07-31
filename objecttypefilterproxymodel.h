#ifndef ENDOSCOPE_OBJECTTYPEFILTERPROXYMODEL_H
#define ENDOSCOPE_OBJECTTYPEFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>
#include "objectlistmodel.h"


namespace Endoscope {

template <typename T>
class ObjectTypeFilterProxyModel : public QSortFilterProxyModel
{
  public:
    explicit ObjectTypeFilterProxyModel(QObject* parent = 0) : QSortFilterProxyModel( parent )
    {
      setDynamicSortFilter( true );
    }

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
    {
      const QModelIndex source_index = sourceModel()->index( source_row, 0, source_parent );
      if ( !source_index.isValid() )
	return false;
      QObject *obj = source_index.data( ObjectListModel::ObjectRole ).value<QObject*>();
      if ( !qobject_cast<T*>( obj ) )
        return false;
      return QSortFilterProxyModel::filterAcceptsRow( source_row, source_parent );
    }
};

}

#endif // ENDOSCOPE_OBJECTTYPEFILTERPROXYMODEL_H
