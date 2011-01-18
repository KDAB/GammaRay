
#ifndef STATEMODEL_H
#define STATEMODEL_H

#include "objectmodelbase.h"

class QStateMachine;

namespace Endoscope
{

class StateModelPrivate;

class StateModel : public ObjectModelBase<QAbstractItemModel>
{
public:
    enum Roles {
      TransitionsRole = Qt::UserRole,
      IsInitialStateRole,
      StateObjectRole = Qt::UserRole + 11
    };
    StateModel(QStateMachine *stateMachine, QObject *parent = 0);
    int rowCount ( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QModelIndex index ( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;

protected:
    Q_DECLARE_PRIVATE(StateModel)
    StateModelPrivate * const d_ptr;
};

}

#endif
