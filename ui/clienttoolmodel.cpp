/*
  clienttoolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "clienttoolmodel.h"
#include "clienttoolmanager.h"

#include <common/endpoint.h>
#include <common/modelroles.h>

#include <QWidget>

using namespace GammaRay;

ClientToolModel::ClientToolModel(ClientToolManager *manager)
    : QAbstractListModel(manager)
    , m_toolManager(manager)
{
    connect(m_toolManager, &ClientToolManager::aboutToReceiveData, this, &ClientToolModel::startReset);
    connect(m_toolManager, &ClientToolManager::toolListAvailable, this, &ClientToolModel::finishReset);
    connect(m_toolManager, &ClientToolManager::aboutToReset, this, &ClientToolModel::startReset);
    connect(m_toolManager, &ClientToolManager::reset, this, &ClientToolModel::finishReset);
    connect(m_toolManager, &ClientToolManager::toolEnabledByIndex, this, &ClientToolModel::toolEnabled);
}

ClientToolModel::~ClientToolModel() = default;

QVariant ClientToolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ToolInfo tool = m_toolManager->tools().at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return tool.name();
    case ToolModelRole::ToolId:
        return tool.id();
    case ToolModelRole::ToolWidget:
        return QVariant::fromValue(m_toolManager->widgetForIndex(index.row()));
    case Qt::ToolTipRole:
        if (!tool.remotingSupported() && Endpoint::instance()->isRemoteClient())
            return tr("This tool does not work in out-of-process mode.");
        return QVariant();
    case ToolModelRole::ToolEnabled:
        return tool.isEnabled();
    case ToolModelRole::ToolHasUi:
        return tool.hasUi();
    case ToolModelRole::ToolFeedbackId: {
        auto id = tool.id().toLower();
        if (id.startsWith(QLatin1String("gammaray_")))
            id = id.mid(9);
        else if (id.startsWith(QLatin1String("gammaray::")))
            id = id.mid(10);
        return id;
    }
    }
    return QVariant();
}

void ClientToolModel::toolEnabled(int toolIndex)
{
    QModelIndex i = index(toolIndex, 0);
    emit dataChanged(i, i, QVector<int>() << ToolModelRole::ToolEnabled);
}

void ClientToolModel::startReset()
{
    beginResetModel();
}

void ClientToolModel::finishReset()
{
    endResetModel();
}

Qt::ItemFlags ClientToolModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (!index.isValid())
        return flags;

    const auto &tool = m_toolManager->tools().at(index.row());
    if (!tool.isEnabled() || (!tool.remotingSupported() && Endpoint::instance()->isRemoteClient()))
        flags &= ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    return flags;
}

int ClientToolModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_toolManager->tools().count();
}


ClientToolSelectionModel::ClientToolSelectionModel(ClientToolManager *manager)
    : QItemSelectionModel(manager->model())
    , m_toolManager(manager)
{
    connect(manager, &ClientToolManager::toolSelectedByIndex, this, &ClientToolSelectionModel::selectTool);
    connect(manager, &ClientToolManager::toolListAvailable, this, &ClientToolSelectionModel::selectDefaultTool);
}

ClientToolSelectionModel::~ClientToolSelectionModel() = default;

void ClientToolSelectionModel::selectTool(int index)
{
    select(model()->index(index, 0), QItemSelectionModel::Select | QItemSelectionModel::Clear | QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void ClientToolSelectionModel::selectDefaultTool()
{
    selectTool(m_toolManager->toolIndexForToolId(QStringLiteral("GammaRay::ObjectInspector")));
}
