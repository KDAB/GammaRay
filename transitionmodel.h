
#ifndef TRANSITIONMODEL_H
#define TRANSITIONMODEL_H

#include "objectmodelbase.h"

class QState;

namespace Endoscope
{

class TransitionModelPrivate;

class TransitionModel : public ObjectModelBase<QAbstractItemModel>
{
public:
    TransitionModel(QObject *parent = 0);
    void setState(QState *state);
    int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    int rowCount ( const QModelIndex& parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QModelIndex index ( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;

protected:
    Q_DECLARE_PRIVATE(TransitionModel)
    TransitionModelPrivate * const d_ptr;
};

}

#endif
