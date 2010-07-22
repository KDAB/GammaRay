#ifndef ENDOSCOPE_OBJECTMODELBASE_H
#define ENDOSCOPE_OBJECTMODELBASE_H

#include <KLocalizedString>
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

    ObjectModelBase<Base>( QObject * parent ) : Base( parent ) {}

    int columnCount(const QModelIndex& parent = QModelIndex()) const
    {
      Q_UNUSED( parent );
      return 2;
    }

    QVariant dataForObject(QObject* obj, const QModelIndex &index, int role) const
    {
      if ( role == Qt::DisplayRole ) {
        if ( index.column() == 0 )
          return obj->objectName().isEmpty() ? (QLatin1String( "0x" ) + QString::number( reinterpret_cast<qlonglong>( obj ), 16 )) : obj->objectName();
        else if ( index.column() == 1 )
          return obj->metaObject()->className();
      } else if ( role == ObjectRole ) {
        return QVariant::fromValue( obj );
      }
      return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
      if ( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        switch ( section ) {
          case 0: return i18n( "Object" );
          case 1: return i18n( "Type" );
        }
      }
      return Base::headerData( section, orientation, role );
    }
};

}

#endif
