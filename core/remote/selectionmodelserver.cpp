/*
  selectionmodelserver.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "selectionmodelserver.h"
#include "server.h"

#include <QTimer>

using namespace GammaRay;

SelectionModelServer::SelectionModelServer(const QString &objectName, QAbstractItemModel *model,
                                           QObject *parent)
    : NetworkSelectionModel(objectName, model, parent)
    , m_timer(new QTimer(this))
    , m_monitored(false)
{
    // We do use a timer to group requests to avoid network overhead
    m_timer->setSingleShot(true);
    m_timer->setInterval(125);
    connect(m_timer, &QTimer::timeout, this, &SelectionModelServer::timeout);

    m_myAddress = Server::instance()->registerObject(objectName, this, Server::ExportNothing);
    Server::instance()->registerMessageHandler(m_myAddress, this, "newMessage");
    Server::instance()->registerMonitorNotifier(m_myAddress, this, "modelMonitored");
    connect(Endpoint::instance(), &Endpoint::disconnected, this, [this] { modelMonitored(); });
}

SelectionModelServer::~SelectionModelServer() = default;

bool SelectionModelServer::isConnected() const
{
    return NetworkSelectionModel::isConnected() && m_monitored;
}

void SelectionModelServer::timeout()
{
    sendSelection();
}

void SelectionModelServer::modelMonitored(bool monitored)
{
    if (m_monitored == monitored)
        return;
    if (m_monitored)
        disconnectModel();
    m_monitored = monitored;
    if (m_monitored)
        connectModel();
}

void SelectionModelServer::connectModel()
{
    Q_ASSERT(model());

    auto startTimer = [this]() { m_timer->start(); };
    connect(model(), &QAbstractItemModel::modelReset, m_timer, startTimer);
    connect(model(), &QAbstractItemModel::rowsInserted, m_timer, startTimer);
    connect(model(), &QAbstractItemModel::rowsMoved, m_timer, startTimer);
    connect(model(), &QAbstractItemModel::columnsInserted, m_timer, startTimer);
    connect(model(), &QAbstractItemModel::columnsMoved, m_timer, startTimer);
    connect(model(), &QAbstractItemModel::layoutChanged, m_timer, startTimer);
}

void SelectionModelServer::disconnectModel()
{
    if (!model())
        return;

    disconnect(model(), &QAbstractItemModel::modelReset, m_timer, nullptr);
    disconnect(model(), &QAbstractItemModel::rowsInserted, m_timer, nullptr);
    disconnect(model(), &QAbstractItemModel::rowsMoved, m_timer, nullptr);
    disconnect(model(), &QAbstractItemModel::columnsInserted, m_timer, nullptr);
    disconnect(model(), &QAbstractItemModel::columnsMoved, m_timer, nullptr);
    disconnect(model(), &QAbstractItemModel::layoutChanged, m_timer, nullptr);
}
