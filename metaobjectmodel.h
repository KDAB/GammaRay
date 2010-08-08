#ifndef ENDOSCOPE_METAOBJECTMODEL_H
#define ENDOSCOPE_METAOBJECTMODEL_H

#include <KLocalizedString>
#include <qabstractitemmodel.h>
#include <QtCore/qsharedpointer.h>
#include <QMetaObject>


namespace Endoscope {

template <typename MetaThing, MetaThing (QMetaObject::*MetaAccessor)(int) const, int (QMetaObject::*MetaCount)() const, int (QMetaObject::*MetaOffset)() const>
class MetaObjectModel : public QAbstractItemModel
{
  public:
    explicit MetaObjectModel(QObject* parent = 0) : QAbstractItemModel( parent ) {}

    virtual void setObject( QObject *object )
    {
      m_object = object;
      reset();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
      if ( !index.isValid() || !m_object || index.row() < 0 || index.row() >= rowCount( index.parent() ) )
        return QVariant();

      const MetaThing metaThing = (m_object.data()->metaObject()->*MetaAccessor)( index.row() );
      if ( index.column() == columnCount( index ) - 1 && role == Qt::DisplayRole ) {
        const QMetaObject* mo = m_object.data()->metaObject();
        while ( (mo->*MetaOffset)() > index.row() )
          mo = mo->superClass();
        return mo->className();
      }
      return data( index, metaThing, role );
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
      if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
        if ( section == columnCount() - 1 )
          return i18n( "Class" );
        return columnHeader( section );
      }
      return QAbstractItemModel::headerData( section, orientation, role );
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
      if ( !m_object || parent.isValid() )
        return 0;
      return (m_object.data()->metaObject()->*MetaCount)();
    }

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
    {
      if ( row >= 0 && row < rowCount( parent ) && column >= 0 && column < columnCount( parent ) && !parent.isValid() )
        return createIndex( row, column, -1 );
      return QModelIndex();
    }

    QModelIndex parent(const QModelIndex& child) const
    {
      return QModelIndex();
    }

  protected:
    virtual QVariant data( const QModelIndex &index, const MetaThing &metaThing, int role ) const = 0;
    virtual QString columnHeader( int index ) const = 0;

  protected:
    QWeakPointer<QObject> m_object;
};

}

#endif // ENDOSCOPE_METAOBJECTMODEL_H
