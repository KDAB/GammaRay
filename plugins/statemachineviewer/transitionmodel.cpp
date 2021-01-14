/*
  transitionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "transitionmodel.h"

#include <core/util.h>

#include <QAbstractTransition>
#include <QDebug>
#include <QSignalTransition>
#include <QState>
#include <QStringList>

#include <algorithm>

// #include <modeltest.h>

using namespace GammaRay;

namespace GammaRay {
class TransitionModelPrivate
{
    explicit TransitionModelPrivate(TransitionModel *qq)
        : q_ptr(qq)
        , m_state(nullptr)
    {
    }

    Q_DECLARE_PUBLIC(TransitionModel)
    TransitionModel * const q_ptr;
    QAbstractState *m_state;

    QList<QObject *> children(QObject *parent) const;

    QObject *mapModelIndex2QObject(const QModelIndex &) const;
};
}

QList<QObject *> TransitionModelPrivate::children(QObject *parent) const
{
    QList<QObject *> result;
    if (parent == nullptr)
        parent = m_state;

    foreach (QObject *o, parent->children()) {
        if (o->inherits("QAbstractTransition"))
            result.append(o);
    }

    std::sort(result.begin(), result.end());
    return result;
}

QObject *TransitionModelPrivate::mapModelIndex2QObject(const QModelIndex &index) const
{
    if (index.isValid()) {
        QObjectList c = children(reinterpret_cast<QObject *>(index.internalPointer()));
        return c[index.row()];
    }
    return m_state;
}

TransitionModel::TransitionModel(QObject *parent)
    : ObjectModelBase<QAbstractItemModel>(parent)
    , d_ptr(new TransitionModelPrivate(this))
{
}

TransitionModel::~TransitionModel()
{
    delete d_ptr;
}

void TransitionModel::setState(QAbstractState *state)
{
    Q_D(TransitionModel);
    beginResetModel();
    d->m_state = state;
    endResetModel();
}

QVariant TransitionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 2)
        return ObjectModelBase<QAbstractItemModel>::headerData(section, orientation, role);
    if (section == 2)
        return "Signal";
    if (section == 3)
        return "Target";
    return QVariant();
}

QVariant TransitionModel::data(const QModelIndex &index, int role) const
{
    Q_D(const TransitionModel);
    if (!index.isValid() || !d->m_state)
        return QVariant();

    QObject *obj = d->mapModelIndex2QObject(index);

    if (obj) {
        if (index.column() == 2 && role == Qt::DisplayRole) {
            QSignalTransition *sigTransition = qobject_cast<QSignalTransition *>(obj);
            if (sigTransition)
                return sigTransition->signal();
        } else if (index.column() == 3 && role == Qt::DisplayRole) {
            QAbstractTransition *transition = qobject_cast<QAbstractTransition *>(obj);
            if (transition)
                return Util::displayString(transition->targetState());
        }
        return dataForObject(obj, index, role);
    }
    return QVariant();
}

int TransitionModel::columnCount(const QModelIndex &parent) const
{
    return ObjectModelBase<QAbstractItemModel>::columnCount(parent) + 2;
}

int TransitionModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const TransitionModel);
    if (!d->m_state)
        return 0;
    return d->children(d->mapModelIndex2QObject(parent)).count();
}

QModelIndex TransitionModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const TransitionModel);

    if (parent.isValid() || !d->m_state)
        return {};

    if (row < 0 || column < 0 || column > 3)
        return QModelIndex();

    QObject *internalPointer = nullptr;
    if (!parent.isValid()) {
        internalPointer = d->m_state;
    } else {
        QObject *o = reinterpret_cast<QObject *>(parent.internalPointer());
        QObjectList c = d->children(o);
        internalPointer = c.at(parent.row());
    }

    QObjectList c = d->children(internalPointer);
    if (row >= c.size())
        return QModelIndex();

    return createIndex(row, column, internalPointer);
}

QModelIndex TransitionModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    // Q_D(const TransitionModel);
    return {};
}
