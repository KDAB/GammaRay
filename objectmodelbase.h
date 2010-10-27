#ifndef ENDOSCOPE_OBJECTMODELBASE_H
#define ENDOSCOPE_OBJECTMODELBASE_H

#include "util.h"

#include <QObject>
#include <QModelIndex>

namespace Endoscope {

template<typename Base>
class ObjectModelBase : public Base
{
  public:
    enum Role {
      ObjectRole = Qt::UserRole +1
    };

    explicit ObjectModelBase<Base>( QObject * parent ) : Base( parent ) {}

    int columnCount(const QModelIndex& parent = QModelIndex()) const
    {
      Q_UNUSED( parent );
      return 2;
    }

    QVariant dataForObject(QObject* obj, const QModelIndex &index, int role) const
    {
      if ( role == Qt::DisplayRole ) {
        if ( index.column() == 0 )
          return obj->objectName().isEmpty() ? Util::addressToString( obj ) : obj->objectName();
        else if ( index.column() == 1 ) {
          return obj->metaObject()->className();
        }
      } else if ( role == ObjectRole ) {
        return QVariant::fromValue( obj );
      } else if ( role == Qt::ToolTipRole ) {
          return QString("Object name: %1\nParent: %2 (Address: %3)\nNumber of children: %4").
            arg( obj->objectName().isEmpty() ? "<Not set>" : obj->objectName() ).
            arg( obj->parent() ? obj->parent()->metaObject()->className() : "<No parent>" ).
            arg( Util::addressToString( obj->parent() ) ).
            arg( obj->children().size() );
      }

      return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
      if ( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        switch ( section ) {
          case 0: return QObject::tr( "Object" );
          case 1: return QObject::tr( "Type" );
        }
      }
      return Base::headerData( section, orientation, role );
    }
};

}

#endif
