/*
  actionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "actionmodel.h"
#include "actionvalidator.h"

#include <core/enumutil.h>
#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>
#include <core/problemcollector.h>
#include <core/objectdataprovider.h>

#include <common/objectid.h>

#include <QAction>
#include <QDebug>
#include <QMutex>
#include <QThread>

Q_DECLARE_METATYPE(QAction::Priority)

using namespace GammaRay;

static QString toString(const QList<QKeySequence> &list)
{
    QStringList items;
    items.reserve(list.size());
    for (const auto &item : list) {
        items << item.toString();
    }
    return items.join(QStringLiteral(", "));
}

ActionModel::ActionModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_duplicateFinder(new ActionValidator(this))
{
    ProblemCollector::registerProblemChecker("gammaray_actioninspector.ShortcutDuplicates",
                                          "Shortcut Duplicates",
                                          "Scans for potential shortcut conflicts in QActions",
                                          [this]() { scanForShortcutDuplicates(); });
}

ActionModel::~ActionModel() = default;

void ActionModel::objectAdded(QObject *object)
{
    // see Probe::objectCreated, that promises a valid object in the main thread
    Q_ASSERT(QThread::currentThread() == thread());
    Q_ASSERT(object);

    QAction * const action = qobject_cast<QAction *>(object);
    if (!action)
        return;

    auto it = std::lower_bound(m_actions.begin(), m_actions.end(), action);
    Q_ASSERT(it == m_actions.end() || *it != action);

    const int row = std::distance(m_actions.begin(), it);
    Q_ASSERT(row >= 0 && row <= m_actions.size());

    beginInsertRows(QModelIndex(), row, row);
    m_actions.insert(it, action);
    Q_ASSERT(m_actions.at(row) == action);
    m_duplicateFinder->insert(action);
    connect(action, &QAction::changed, this, &ActionModel::actionChanged);
    endInsertRows();
}

void ActionModel::objectRemoved(QObject *object)
{
    Q_ASSERT(thread() == QThread::currentThread());
    QAction * const action = reinterpret_cast<QAction *>(object); // never dereference this, just use for comparison

    auto it = std::lower_bound(m_actions.begin(),
                                                       m_actions.end(),
                                                       reinterpret_cast<QAction *>(object));
    if (it == m_actions.end() || *it != action)
        return;

    const int row = std::distance(m_actions.begin(), it);
    Q_ASSERT(row >= 0 && row < m_actions.size());
    Q_ASSERT(m_actions.at(row) == action);

    beginRemoveRows(QModelIndex(), row, row);
    m_actions.erase(it);
    m_duplicateFinder->remove(action);
    endRemoveRows();
}

int ActionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

int ActionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_actions.size();
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QMutexLocker lock(Probe::objectLock());
    QAction *action = m_actions.at(index.row());
    if (!Probe::instance()->isValidObject(action))
        return QVariant();

    const int column = index.column();
    if (role == Qt::DisplayRole) {
        switch (column) {
        case AddressColumn:
            return Util::shortDisplayString(action);
        case NameColumn:
            return action->text();
        case CheckablePropColumn:
            return action->isCheckable();
        case PriorityPropColumn:
            return EnumUtil::enumToString(action->priority(), nullptr, action->metaObject());
        case ShortcutsPropColumn:
            return toString(action->shortcuts());
        default:
            return QVariant();
        }
    } else if (role == Qt::DecorationRole) {
        if (column == NameColumn)
            return action->icon();
    } else if (role == Qt::CheckStateRole) {
        switch (column) {
            case AddressColumn:
                return action->isEnabled() ? Qt::Checked : Qt::Unchecked;
            case CheckedPropColumn:
                if (action->isCheckable())
                    return action->isChecked() ? Qt::Checked : Qt::Unchecked;
                return QVariant();
        }
    } else if (role == ShortcutConflictRole && column == ShortcutsPropColumn) {
        return m_duplicateFinder->hasAmbiguousShortcut(action);
    } else if (role == ActionModel::ObjectRole) {
        return QVariant::fromValue<QObject*>(action);
    } else if (role == ActionModel::ObjectIdRole && index.column() == 0) {
        return QVariant::fromValue(ObjectId(action));
    }

    return QVariant();
}

Qt::ItemFlags ActionModel::flags(const QModelIndex& index) const
{
    const auto f = QAbstractTableModel::flags(index);
    if (!index.isValid())
        return f;
    if (index.column() == AddressColumn)
        return f | Qt::ItemIsUserCheckable;
    if (index.column() == CheckedPropColumn && m_actions.at(index.row())->isCheckable())
        return f | Qt::ItemIsUserCheckable;
    return f;
}

bool ActionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid()) {
        auto action = m_actions.at(index.row());
        switch (index.column()) {
            case AddressColumn:
                action->setEnabled(value.toInt() == Qt::Checked);
                return true;
            case CheckedPropColumn:
                action->setChecked(value.toInt() == Qt::Checked);
                return true;
        }
    }
    return QAbstractItemModel::setData(index, value, role);
}

void ActionModel::actionChanged()
{
    auto action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    auto row = m_actions.indexOf(action);
    emit dataChanged(index(row, 0), index(row, ActionModel::ShortcutsPropColumn));
}

void ActionModel::scanForShortcutDuplicates() const
{
    for (QAction *action : m_actions) {
        Q_FOREACH (const QKeySequence &sequence, m_duplicateFinder->findAmbiguousShortcuts(action)) {
            Problem p;
            p.severity = Problem::Error;
            p.description = QStringLiteral("Key sequence %1 is ambigous.").arg(sequence.toString(QKeySequence::NativeText));
            p.problemId = QStringLiteral("gammaray_actioninspector.ShortcutDuplicates:%1").arg(sequence.toString(QKeySequence::PortableText));
            p.object = ObjectId(action);
            p.locations.push_back(ObjectDataProvider::creationLocation(action));
            p.findingCategory = Problem::Scan;
            ProblemCollector::addProblem(p);
        }
    }
}
