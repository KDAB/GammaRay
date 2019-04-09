/*
  availablecheckers.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation problem.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "availablecheckersmodel.h"


using namespace GammaRay;


AvailableCheckersModel::AvailableCheckersModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_availableCheckers(&ProblemCollector::instance()->availableCheckers())
{
    connect(ProblemCollector::instance(), &ProblemCollector::aboutToAddChecker, this, &AvailableCheckersModel::aboutToAddChecker);
    connect(ProblemCollector::instance(), &ProblemCollector::checkerAdded, this, &AvailableCheckersModel::checkerAdded);
}

QVariant AvailableCheckersModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= m_availableCheckers->size()) {
        return QVariant();
    }

    const ProblemCollector::Checker &checker = m_availableCheckers->at(index.row());
    if (role == Qt::DisplayRole) {
        return checker.name;
    }
    if (role == Qt::ToolTipRole) {
        return checker.description;
    }
    if (role == Qt::EditRole) {
        return checker.id;
    }
    if (role == Qt::CheckStateRole) {
        return checker.enabled ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

bool GammaRay::AvailableCheckersModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()
        || index.column() != 0
        || index.row() < 0 || index.row() >= m_availableCheckers->size()
        || role != Qt::CheckStateRole
        || !value.canConvert<bool>()) {
        return false;
    }

    (*m_availableCheckers)[index.row()].enabled = value.toBool();
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags AvailableCheckersModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (index.column() == 0)
        return flags | Qt::ItemIsUserCheckable;
    return flags;
}

int AvailableCheckersModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_availableCheckers->size();
}

void GammaRay::AvailableCheckersModel::aboutToAddChecker()
{
    beginInsertRows(QModelIndex(), m_availableCheckers->size(), m_availableCheckers->size());
}
void GammaRay::AvailableCheckersModel::checkerAdded()
{
    endInsertRows();
}

