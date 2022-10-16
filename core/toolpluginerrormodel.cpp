/*
  toolpluginerrormodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "toolpluginerrormodel.h"

using namespace GammaRay;

ToolPluginErrorModel::ToolPluginErrorModel(const PluginLoadErrors &errors, QObject *parent)
    : QAbstractTableModel(parent)
    , m_errors(errors)
{
}

ToolPluginErrorModel::~ToolPluginErrorModel() = default;

int ToolPluginErrorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int ToolPluginErrorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_errors.size();
}

QVariant ToolPluginErrorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    const int column = index.column();
    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0:
            return m_errors[row].pluginName();
        case 1:
            return m_errors[row].pluginFile;
        case 2:
            return m_errors[row].errorString;
        }
    }
    return QVariant();
}

QVariant ToolPluginErrorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Plugin Name");
        case 1:
            return tr("Plugin File");
        case 2:
            return tr("Error Message");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
