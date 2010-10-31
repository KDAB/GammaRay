#ifndef MODELMODEL_H
#define MODELMODEL_H

#include "objectmodelbase.h"
#include <qabstractitemmodel.h>
#include <qabstractproxymodel.h>
#include <qpointer.h>
#include <QVector>

namespace Endoscope {

class ModelModel : public ObjectModelBase<QAbstractItemModel>
{
  Q_OBJECT
  public:
    explicit ModelModel( QObject *parent );
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    void objectRemoved( QObject *obj );

  public slots:
    void objectAdded( const QPointer<QObject> &obj );

  private:
    QModelIndex indexForModel( QAbstractItemModel* model ) const;
    QVector<QAbstractProxyModel*> proxiesForModel( QAbstractItemModel* model ) const;

  private:
    QVector<QAbstractItemModel*> m_models;
    QVector<QAbstractProxyModel*> m_proxies;
};

}

#endif // MODELMODEL_H
