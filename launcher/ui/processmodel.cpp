/*
  processmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "processmodel.h"
#include "probeabimodel.h"

#include <compat/qasconst.h>

#include <launcher/core/probefinder.h>

#include <QDebug>

#include <algorithm>

using namespace GammaRay;

bool operator<(const ProcData &l, const ProcData &r)
{
    return l.ppid < r.ppid;
}

bool operator==(const ProcData &l, const ProcData &r)
{
    return l.ppid == r.ppid;
}

QDebug operator<<(QDebug d, const ProcData &data)
{
    d << "ProcData{.ppid=" << data.ppid << ", .name=" << data.name << ", .image=" << data.image
      << ", .state=" << data.state << ", .user=" << data.user << ", .type=" << data.abi.id() << "}";
    return d;
}

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_availableABIs = ProbeFinder::listProbeABIs();
}

ProcessModel::~ProcessModel() = default;

void ProcessModel::setProcesses(const ProcDataList &processes)
{
    beginResetModel();
    m_data = processes;
    // sort for merging to work properly
    std::stable_sort(m_data.begin(), m_data.end());
    endResetModel();
}

void ProcessModel::mergeProcesses(const ProcDataList &processes)
{
    // sort like m_data
    ProcDataList sortedProcesses = processes;
    std::stable_sort(sortedProcesses.begin(), sortedProcesses.end());

    // iterator over m_data
    int i = 0;

    for (const ProcData &newProc : qAsConst(sortedProcesses)) {
        bool shouldInsert = true;
        while (i < m_data.count()) {
            const ProcData &oldProc = m_data.at(i);
            if (oldProc < newProc) {
                // remove old proc, seems to be outdated
                beginRemoveRows(QModelIndex(), i, i);
                m_data.removeAt(i);
                endRemoveRows();
                continue;
            } else if (newProc == oldProc) {
                // already contained, hence increment and break.
                // Update entry before if something changed (like state),
                // this make sure m_data match exactly sortedProcesses for later Q_ASSERT check.
                if (!newProc.equals(oldProc)) {
                    m_data[i] = newProc;
                    emit dataChanged(index(i, 0), index(i, columnCount() - 1));
                }
                ++i;
                shouldInsert = false;
                break;
            } else { // newProc < oldProc
                // new entry, break and insert it
                break;
            }
        }
        if (shouldInsert) {
            beginInsertRows(QModelIndex(), i, i);
            m_data.insert(i, newProc);
            endInsertRows();
            // let i point to old element again
            ++i;
        }
    }

    // make sure the new data is properly inserted
    Q_ASSERT(m_data == sortedProcesses);
}

void ProcessModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_data.count());
    m_data.clear();
    endRemoveRows();
}

ProcData ProcessModel::dataForIndex(const QModelIndex &index) const
{
    return m_data.at(index.row());
}

ProcData ProcessModel::dataForRow(int row) const
{
    return m_data.at(row);
}

QModelIndex ProcessModel::indexForPid(const QString &pid) const
{
    for (int i = 0; i < m_data.size(); ++i) {
        if (m_data.at(i).ppid == pid)
            return index(i, 0);
    }
    return {};
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    if (section == PIDColumn)
        return tr("Process ID");
    else if (section == NameColumn)
        return tr("Name");
    else if (section == StateColumn)
        return tr("State");
    else if (section == UserColumn)
        return tr("User");

    return QVariant();
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ProcData &data = m_data.at(index.row());

    if (role == Qt::DisplayRole) {
        if (index.column() == PIDColumn)
            return data.ppid;
        else if (index.column() == NameColumn)
            return data.image.isEmpty() ? data.name : data.image;
        else if (index.column() == StateColumn)
            return data.state;
        else if (index.column() == UserColumn)
            return data.user;
    } else if (role == Qt::ToolTipRole) {
        const ProbeABI bestABI = ProbeFinder::findBestMatchingABI(data.abi, m_availableABIs);
        return tr("Name: %1\nPID: %2\nOwner: %3\nQt ABI: %4\nProbe available: %5").arg(data.image.isEmpty() ? data.name : data.image, data.ppid, data.user, data.abi.displayString(), bestABI.isValid() ? tr("yes") : tr("no"));
    } else if (role == PIDRole) {
        return data.ppid.toInt(); // why is this a QString in the first place!?
    } else if (role == NameRole) {
        return data.image.isEmpty() ? data.name : data.image;
    } else if (role == StateRole) {
        return data.state;
    } else if (role == UserRole) {
        return data.user;
    } else if (role == ABIRole) {
        return QVariant::fromValue(data.abi);
    }

    return QVariant();
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.count();
}

ProcDataList ProcessModel::processes() const
{
    return m_data;
}

Qt::ItemFlags ProcessModel::flags(const QModelIndex &index) const
{
    const ProbeABI abi = index.data(ABIRole).value<ProbeABI>();
    const ProbeABI bestABI = ProbeFinder::findBestMatchingABI(abi, m_availableABIs);

    const Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (!bestABI.isValid())
        return f & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    return f;
}
