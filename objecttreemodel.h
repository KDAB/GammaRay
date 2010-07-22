#ifndef ENDOSCOPE_OBJECTTREEMODEL_H
#define ENDOSCOPE_OBJECTTREEMODEL_H

#include "objectmodelbase.h"
#include <qabstractitemmodel.h>
#include <qvector.h>


namespace Endoscope {

class ObjectTreeModel : public ObjectModelBase<QAbstractItemModel>
{
  Q_OBJECT
  public:
    ObjectTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    void objectAdded( QObject *obj );
    void objectRemoved( QObject *obj );

  protected:
    bool eventFilter(QObject *receiver, QEvent *event);

  private:
    QModelIndex indexForObject( QObject* object ) const;

  private:
    QMap<QObject*, QObject*> m_childParentMap;
    QMap<QObject*, QVector<QObject*> > m_parentChildMap;
};

}

#endif // ENDOSCOPE_OBJECTTREEMODEL_H
