/*
  statemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "statemodel.h"
#include "statemachinedebuginterface.h"
#include "statemachinewatcher.h"

#include <compat/qasconst.h>

#include <core/objectmodelbase.h>
#include <core/util.h>

#include <QAbstractTransition>
#include <QDebug>
#include <QStateMachine>
#include <QStringList>

#include <algorithm>

// #include <modeltest.h>

using namespace GammaRay;

namespace GammaRay {
class StateModelPrivate
{
    explicit StateModelPrivate(StateModel *qq)
        : q_ptr(qq)
        , m_stateMachine(nullptr)
    {
    }

    void emitDataChangedForState(State state)
    {
        const auto left = indexForState(state);
        const auto right = left.sibling(left.row(), q_ptr->columnCount() - 1);
        if (!left.isValid() || !right.isValid())
            return;
        emit q_ptr->dataChanged(left, right);
    }

    Q_DECLARE_PUBLIC(StateModel)
    StateModel * const q_ptr;
    StateMachineDebugInterface *m_stateMachine;
    QVector<State> m_lastConfiguration;

    QVector<State> children(State parent) const;

    State mapModelIndex2State(const QModelIndex &) const;
    QModelIndex indexForState(State state) const;

// private slots:
    void stateConfigurationChanged();
    void handleMachineDestroyed(QObject *);
};
}

QVector<State> StateModelPrivate::children(State parent) const
{
    if (!m_stateMachine)
        return QVector<State>();

    return m_stateMachine->stateChildren(parent);
}

State StateModelPrivate::mapModelIndex2State(const QModelIndex &index) const
{
    if (!m_stateMachine)
        return State();

    if (index.isValid()) {
        QVector<State> c = children(State(index.internalId()));
        return c[index.row()];
    }
    return m_stateMachine->rootState();
}

QModelIndex StateModelPrivate::indexForState(State state) const
{
    if (!m_stateMachine)
        return {};

    if (state == m_stateMachine->rootState())
        return QModelIndex();

    Q_Q(const StateModel);
    State parentState = m_stateMachine->parentState(state);
    int row = m_stateMachine->stateChildren(parentState).indexOf(state);
    if (row == -1)
        return QModelIndex();
    return q->index(row, 0, indexForState(parentState));
}

void StateModelPrivate::stateConfigurationChanged()
{
    QVector<State> newConfig = m_stateMachine->configuration();
    // states which became active
    QVector<State> difference;
    std::set_difference(newConfig.begin(), newConfig.end(),
                        m_lastConfiguration.begin(), m_lastConfiguration.end(),
                        std::back_inserter(difference));
    for (State state : qAsConst(difference))
        emitDataChangedForState(state);
    // states which became inactive
    difference.clear();
    std::set_difference(m_lastConfiguration.begin(), m_lastConfiguration.end(),
                        newConfig.begin(), newConfig.end(),
                        std::back_inserter(difference));
    for (State state : qAsConst(difference))
        emitDataChangedForState(state);
    m_lastConfiguration = newConfig;
}

void StateModelPrivate::handleMachineDestroyed(QObject *)
{
    Q_Q(StateModel);

    q->beginResetModel();
    m_stateMachine = nullptr;
    q->endResetModel();
}

StateModel::StateModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new StateModelPrivate(this))
{
}

StateModel::~StateModel()
{
    delete d_ptr;
}

void StateModel::setStateMachine(StateMachineDebugInterface *stateMachine)
{
    Q_D(StateModel);
    if (d->m_stateMachine == stateMachine)
        return;

    if (d->m_stateMachine) {
        disconnect(d->m_stateMachine, nullptr, this, nullptr);
    }

    beginResetModel();
    d->m_stateMachine = stateMachine;
    d->m_lastConfiguration = (stateMachine ? stateMachine->configuration() : QVector<State>());
    endResetModel();

    if (d->m_stateMachine) {
        connect(d->m_stateMachine, &QObject::destroyed,
                this, [this](QObject *obj) { Q_D(StateModel); d->handleMachineDestroyed(obj); });
        connect(d->m_stateMachine, &StateMachineDebugInterface::stateEntered,
                this, [this] { Q_D(StateModel); d->stateConfigurationChanged(); });
        connect(d->m_stateMachine, &StateMachineDebugInterface::stateEntered,
                this, [this] { Q_D(StateModel); d->stateConfigurationChanged(); });
    }
}

StateMachineDebugInterface *StateModel::stateMachine() const
{
    Q_D(const StateModel);
    return d->m_stateMachine;
}

QVariant StateModel::data(const QModelIndex &index, int role) const
{
    Q_D(const StateModel);
    if (!index.isValid())
        return QVariant();

    State state = d->mapModelIndex2State(index);
    QObject *object = d->m_stateMachine->stateObject(state);

    if (role == TransitionsRole) {
        return d->m_stateMachine->transitions(state);
    } else if (role == IsInitialStateRole) {
        return d->m_stateMachine->isInitialState(state);
    } else if (role == StateValueRole) {
        return QVariant::fromValue(state);
    } else if (role == StateIdRole) {
        return QVariant::fromValue(GammaRay::StateId(static_cast<quint64>(state.m_id)));
    } else if (role == Qt::CheckStateRole && index.column() == 0) {
        return d->m_stateMachine->configuration().contains(state) ? Qt::Checked : Qt::Unchecked;
    } else if (role == Qt::DisplayRole && index.column() == 0) {
        return d->m_stateMachine->stateDisplay(state);
    } else if (role == Qt::DisplayRole && index.column() == 1) {
        return d->m_stateMachine->stateDisplayType(state);
    } else if (role == ObjectModel::ObjectRole) {
        return QVariant::fromValue(object);
    } else if (role == ObjectModel::ObjectIdRole) {
        return QVariant::fromValue(ObjectId(object));
    } else if (role == Qt::ToolTipRole) {
        return Util::tooltipForObject(object);
    } else if (role == ObjectModel::DecorationIdRole && index.column() == 0) {
        return Util::iconIdForObject(object);
    } else if (role == ObjectModel::CreationLocationRole) {
        const auto loc = ObjectDataProvider::creationLocation(object);
        if (loc.isValid())
            return QVariant::fromValue(loc);
    } else if (role == ObjectModel::DeclarationLocationRole) {
        const auto loc = ObjectDataProvider::declarationLocation(object);
        if (loc.isValid())
            return QVariant::fromValue(loc);
    }

    return QVariant();
}

int StateModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const StateModel);
    return d->children(d->mapModelIndex2State(parent)).count();
}

QModelIndex StateModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const StateModel);
    if (row < 0 || column < 0 || column > 1)
        return {};

    State internalPointer(0);
    if (!parent.isValid()) {
        internalPointer = d->m_stateMachine->rootState();
    } else {
        State s = State(parent.internalId());
        QVector<State> c = d->m_stateMachine->stateChildren(s);
        internalPointer = c.at(parent.row());
    }

    QVector<State> c = d->children(internalPointer);
    if (row >= c.size())
        return QModelIndex();

    return createIndex(row, column, internalPointer);
}

QModelIndex StateModel::parent(const QModelIndex &index) const
{
    Q_D(const StateModel);
    if (!index.isValid() || !d->m_stateMachine)
        return {};
    State state = d->mapModelIndex2State(index);
    State parent = d->m_stateMachine->parentState(state);

    if (parent == d->m_stateMachine->rootState())
        return QModelIndex();

    State grandParent = d->m_stateMachine->parentState(parent);
    int row = d->children(grandParent).indexOf(parent);
    return createIndex(row, 0, grandParent);
}

QVariant StateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("State");
        case 1:
            return tr("Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

int StateModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QMap<int, QVariant> StateModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> map = QAbstractItemModel::itemData(index);
    map.insert(ObjectModel::ObjectIdRole, this->data(index, ObjectModel::ObjectIdRole));
    map.insert(ObjectModel::DecorationIdRole, this->data(index, ObjectModel::DecorationIdRole));
    auto loc = this->data(index, ObjectModel::CreationLocationRole);
    if (loc.isValid())
        map.insert(ObjectModel::CreationLocationRole, loc);
    loc = this->data(index, ObjectModel::DeclarationLocationRole);
    if (loc.isValid())
        map.insert(ObjectModel::DeclarationLocationRole, loc);
    return map;
}

QHash<int, QByteArray> StateModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(TransitionsRole, "transitions");
    roleNames.insert(IsInitialStateRole, "isInitial");
    return roleNames;
}

#include "moc_statemodel.cpp"
