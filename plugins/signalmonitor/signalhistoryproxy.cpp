/*
  signalhistoryproxy.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "signalhistoryproxy.h"
#include "signalhistorymodel.h"

#include <common/metatypedeclarations.h>

#include <QTimer>

using namespace GammaRay;

SignalHistoryProxy::SignalHistoryProxy(QObject *parent)
    : SignalHistoryBaseProxy(parent)
    , m_headerMonitorCheckState(Qt::Unchecked)
    , m_checkStateHeaderChangedTimer(new QTimer(this))
{
    m_checkStateHeaderChangedTimer->setSingleShot(true);
    m_checkStateHeaderChangedTimer->setInterval(250);

    connect(this, SIGNAL(modelReset()), m_checkStateHeaderChangedTimer, SLOT(start()));
    connect(this, SIGNAL(layoutChanged()), m_checkStateHeaderChangedTimer, SLOT(start()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), m_checkStateHeaderChangedTimer, SLOT(start()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), m_checkStateHeaderChangedTimer, SLOT(start()));
    connect(this, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), m_checkStateHeaderChangedTimer, SLOT(start()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));
#else
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
#endif
    connect(m_checkStateHeaderChangedTimer, SIGNAL(timeout()), this, SLOT(updateHeaderCheckState()));
}

QVariant SignalHistoryProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal &&
            section == SignalHistoryModel::MonitoredColumn &&
            role == Qt::CheckStateRole) {
        return m_headerMonitorCheckState;
    }

    return SignalHistoryBaseProxy::headerData(section, orientation, role);
}

bool SignalHistoryProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal &&
            section == SignalHistoryModel::MonitoredColumn &&
            role == Qt::CheckStateRole) {
        // If we are not filtering the model, let use the source model fallback
        // wich is able to update all at once without too much overhead
        // else update the visible indexes only.
        // This allow to select only the filtered group of indexes instead of all.
        if (filterRegExp().pattern().isEmpty()) {
            return SignalHistoryBaseProxy::setHeaderData(section, orientation, value, role);
        } else {
            const Qt::CheckState newState = value.value<Qt::CheckState>();
            const bool oldBlockingState = blockSignals(true);
            const int count = rowCount();
            QVector<QPair<int, int>> ranges; // pair of first/last

            for (int i = 0; i < count; ++i) {
                const QModelIndex index = SignalHistoryBaseProxy::index(i, section);

                if (index.data(role).value<Qt::CheckState>() != newState) {
                    if (setData(index, newState, role)) {
                        if (ranges.isEmpty() || ranges.last().second != i - 1) {
                            ranges << qMakePair(i, i);
                        } else {
                            ranges.last().second = i;
                        }
                    }
                }
            }

            blockSignals(oldBlockingState);

            foreach (const auto &range, ranges) {
                emit dataChanged(index(range.first, section), index(range.second, section));
            }
        }
    }

    return false;
}

void SignalHistoryProxy::sort(int column, Qt::SortOrder order)
{
    if (static_cast<SignalHistoryModel::ColumnId>(column) == SignalHistoryModel::MonitoredColumn) {
        setSortRole(Qt::CheckStateRole);
    } else {
        setSortRole(Qt::DisplayRole);
    }

    SignalHistoryBaseProxy::sort(column, order);
}

void SignalHistoryProxy::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if ((topLeft.column() == SignalHistoryModel::MonitoredColumn ||
         bottomRight.column() == SignalHistoryModel::MonitoredColumn) &&
            (roles.isEmpty() || roles.contains(Qt::CheckStateRole))) {
        m_checkStateHeaderChangedTimer->start();
    }
}

void SignalHistoryProxy::updateHeaderCheckState()
{
    if (!sourceModel())
        return;

    Qt::CheckState newState = m_headerMonitorCheckState;

    if (filterRegExp().pattern().isEmpty()) {
        newState = sourceModel()->headerData(SignalHistoryModel::MonitoredColumn,
                                                                  Qt::Horizontal, Qt::CheckStateRole).value<Qt::CheckState>();
    } else {
        const int count = rowCount();
        int checked = 0;

        for (int i = 0; i < count; ++i) {
            const QModelIndex index = SignalHistoryBaseProxy::index(i, SignalHistoryModel::MonitoredColumn);

            if (index.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked) {
                ++checked;
            }
        }

        newState = Qt::Unchecked;

        if (checked > 0) {
            newState = checked == count ? Qt::Checked : Qt::PartiallyChecked;
        }
    }

    if (newState != m_headerMonitorCheckState) {
        m_headerMonitorCheckState = newState;
        emit headerDataChanged(Qt::Horizontal, SignalHistoryModel::MonitoredColumn,
                               SignalHistoryModel::MonitoredColumn);
    }
}
