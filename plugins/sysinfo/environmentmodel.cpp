/*
  environmentmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "environmentmodel.h"

using namespace GammaRay;

EnvironmentModel::EnvironmentModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_env(QProcessEnvironment::systemEnvironment())
{
}

EnvironmentModel::~EnvironmentModel() = default;

int EnvironmentModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int EnvironmentModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_env.keys().size();
}

QVariant EnvironmentModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return m_env.keys().at(index.row());
            case 1: return m_env.value(m_env.keys().at(index.row()));
        }
    }

    return QVariant();
}

QVariant EnvironmentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Environment Variable");
            case 1:
                return tr("Value");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
