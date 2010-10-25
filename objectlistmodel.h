#ifndef ENDOSCOPE_OBJECTLISTMODEL_H
#define ENDOSCOPE_OBJECTLISTMODEL_H

#include "objectmodelbase.h"
#include <qvector.h>
#include <QPointer>

namespace Endoscope {

class ObjectListModel : public ObjectModelBase<QAbstractTableModel>
{
  Q_OBJECT
  public:
    explicit ObjectListModel( QObject *parent = 0 );
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    
    void objectRemoved( QObject *obj );

  public slots:
    void objectAdded( const QPointer<QObject> &objPtr );

  private:
    QVector<QObject*> m_objects;
};

}

#endif // ENDOSCOPE_OBJECTLISTMODEL_H
