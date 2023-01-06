/*
  toolpluginmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "toolpluginmodel.h"
#include "toolfactory.h"

using namespace GammaRay;

ToolPluginModel::ToolPluginModel(const QVector<ToolFactory *> &plugins, QObject *parent)
    : QAbstractTableModel(parent)
    , m_tools(plugins)
{
}

ToolPluginModel::~ToolPluginModel() = default;

int ToolPluginModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int ToolPluginModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_tools.size();
}

QVariant ToolPluginModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    const int column = index.column();
    if (role == Qt::DisplayRole) {
        ToolFactory *factory = m_tools[row];
        switch (column) {
        case 0:
            return factory->id();
        case 1:
            return factory->supportedTypesString();
        }
    }
    return QVariant();
}

QVariant ToolPluginModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Id");
        case 1:
            return tr("Supported types");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
