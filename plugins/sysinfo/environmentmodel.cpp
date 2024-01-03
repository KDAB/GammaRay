/*
  environmentmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "environmentmodel.h"

using namespace GammaRay;

EnvironmentModel::EnvironmentModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_env(QProcessEnvironment::systemEnvironment())
{
}

EnvironmentModel::~EnvironmentModel() = default;

int EnvironmentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int EnvironmentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_env.keys().size();
}

QVariant EnvironmentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return m_env.keys().at(index.row());
        case 1:
            return m_env.value(m_env.keys().at(index.row()));
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
