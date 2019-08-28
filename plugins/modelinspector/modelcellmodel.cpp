/*
  modelcellmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelcellmodel.h"

#include <compat/qasconst.h>

#include <core/varianthandler.h>

#include <QAbstractProxyModel>

using namespace GammaRay;

ModelCellModel::ModelCellModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

static bool sourceIsQQmlListModel(const QAbstractItemModel *model)
{
    Q_ASSERT(model);
    while (auto proxy = qobject_cast<const QAbstractProxyModel *>(model))
        model = proxy->sourceModel();
    return model->inherits("QQmlListModel");
}

void ModelCellModel::setModelIndex(const QModelIndex &idx)
{
    const auto newRoles = rolesForModel(idx.model());
    if (newRoles != m_roles) {
        if (!m_roles.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, m_roles.size() - 1);
            m_roles.clear();
            endRemoveRows();
        }
        m_index = idx;
        if (!newRoles.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, newRoles.size() - 1);
            m_roles = newRoles;
            endInsertRows();
        }
    } else {
        m_index = idx;
        if (!m_roles.isEmpty()) {
            emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
        }
    }
}

QVector<ModelCellModel::RoleInfo> ModelCellModel::rolesForModel(const QAbstractItemModel *model)
{
    QVector<RoleInfo> roles;
    if (!model)
        return roles;

    // add built-in roles
    const auto hasDefaultRoles = !sourceIsQQmlListModel(model);
    if (hasDefaultRoles) {
#define R(x) qMakePair<int, QString>(x, QStringLiteral(#x))
        roles << R(Qt::DisplayRole)
              << R(Qt::DecorationRole)
              << R(Qt::EditRole)
              << R(Qt::ToolTipRole)
              << R(Qt::StatusTipRole)
              << R(Qt::WhatsThisRole)
              << R(Qt::FontRole)
              << R(Qt::TextAlignmentRole)
              << R(Qt::BackgroundRole)
              << R(Qt::ForegroundRole)
              << R(Qt::CheckStateRole)
              << R(Qt::AccessibleTextRole)
              << R(Qt::AccessibleDescriptionRole)
              << R(Qt::SizeHintRole)
              << R(Qt::InitialSortOrderRole)
          ;
#undef R
    }

    // add custom roles
    const auto roleNames = model->roleNames();

    for (auto it = roleNames.constBegin(); it != roleNames.constEnd(); ++it) {
        bool roleFound = false;
        for (const auto &role : qAsConst(roles)) {
            if (role.first == it.key()) {
                roleFound = true;
                break;
            }
        }
        if (!roleFound) {
            const auto name = it.value().isEmpty() ? tr("Role #%1").arg(it.key()) : QString::fromLatin1(it.value());
            roles.push_back(qMakePair(it.key(), name));
        }
    }

    std::sort(roles.begin(), roles.end(), [] (const RoleInfo& lhs, const RoleInfo& rhs){return lhs.first < rhs.first;});
    return roles;
}

QVariant ModelCellModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Q_ASSERT(index.row() < m_roles.size());
    const QVariant value = m_index.data(m_roles.at(index.row()).first);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return m_roles.at(index.row()).second;
        case 1:
            return VariantHandler::displayString(value);
        case 2:
            return value.typeName();
        }
    } else if (role == Qt::EditRole) {
        if (index.column() == 1)
            return value;
    } else if (role == Qt::DecorationRole && index.column() == 1) {
        return VariantHandler::decoration(value);
    }

    return QVariant();
}

bool ModelCellModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && m_index.isValid()
        && (m_index.flags() & Qt::ItemIsEditable)
        && role == Qt::EditRole && index.column() == 1) {
        const Qt::ItemDataRole sourceRole
            = static_cast<Qt::ItemDataRole>(m_roles.at(index.row()).first);
        QAbstractItemModel *sourceModel = const_cast<QAbstractItemModel *>(m_index.model());
        return sourceModel->setData(m_index, value, sourceRole);
    }
    return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags ModelCellModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.isValid() && m_index.isValid() && index.column() == 1) {
        if (m_index.flags() & Qt::ItemIsEditable)
            return flags | Qt::ItemIsEditable;
    }
    return flags;
}

int ModelCellModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3;
}

int ModelCellModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_index.isValid())
        return 0;
    return m_roles.size();
}

QVariant ModelCellModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Role");
        case 1:
            return tr("Value");
        case 2:
            return tr("Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
