/*
  statemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMODEL_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMODEL_H

#include <common/objectmodel.h>

#include <QAbstractItemModel>

QT_BEGIN_NAMESPACE
class QAbstractTransition;
class QStateMachine;
QT_END_NAMESPACE

namespace GammaRay {
class StateModelPrivate;
class StateMachineDebugInterface;

class StateModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles
    {
        TransitionsRole = ObjectModel::UserRole + 1, ///< return bool, see StateMachineDebugInterface::transitions()
        IsInitialStateRole, ///< return bool, see StateMachineDebugInterface::isInitialState()
        StateValueRole, ///< return GammaRay::State
        StateIdRole ///< return GammaRay::StateId
    };

    explicit StateModel(QObject *parent = nullptr);
    ~StateModel() override;

    void setStateMachine(StateMachineDebugInterface *stateMachine);
    StateMachineDebugInterface *stateMachine() const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

protected:
    Q_DECLARE_PRIVATE(StateModel)
    StateModelPrivate *const d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void stateConfigurationChanged())
    Q_PRIVATE_SLOT(d_func(), void handleMachineDestroyed(QObject *))
};
}

#endif
