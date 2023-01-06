/*
  selectionmodelclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "selectionmodelclient.h"
#include "client.h"

#include <QTimer>

using namespace GammaRay;

SelectionModelClient::SelectionModelClient(const QString &objectName, QAbstractItemModel *model,
                                           QObject *parent)
    : NetworkSelectionModel(objectName, model, parent)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(125);
    Q_ASSERT(model);
    // We do use a timer to group requests to avoid network overhead
    auto startTimer = [this]() { m_timer->start(); };
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &SelectionModelClient::clearPendingSelection);
    connect(model, &QAbstractItemModel::rowsInserted, m_timer, startTimer);
    connect(model, &QAbstractItemModel::rowsMoved, m_timer, startTimer);
    connect(model, &QAbstractItemModel::columnsInserted, m_timer, startTimer);
    connect(model, &QAbstractItemModel::columnsMoved, m_timer, startTimer);
    connect(model, &QAbstractItemModel::layoutChanged, m_timer, startTimer);
    connect(m_timer, &QTimer::timeout, this, &SelectionModelClient::timeout);

    m_myAddress = Client::instance()->objectAddress(objectName);
    connect(Client::instance(), &Endpoint::objectRegistered,
            this, &SelectionModelClient::serverRegistered);
    connect(Client::instance(), &Endpoint::objectUnregistered,
            this, &SelectionModelClient::serverUnregistered);
    connectToServer();
}

SelectionModelClient::~SelectionModelClient()
{
    m_timer->stop();
}

void SelectionModelClient::connectToServer()
{
    if (m_myAddress == Protocol::InvalidObjectAddress)
        return;
    Client::instance()->registerMessageHandler(m_myAddress, this, "newMessage");
    // There can be some delay between the selection model is created and its connections went done in the UI part.
    // So, let delay the request a bit, not perfect but can help.
    // Probably a better way would be to consider some GammaRay::Message to be pendable and put them in a pool
    // to be handled again later.
    // connectionEstablished does not seems to help here (bader).
    QTimer::singleShot(125, this, &SelectionModelClient::requestSelection);
}

void SelectionModelClient::timeout()
{
    applyPendingSelection();
}

void SelectionModelClient::serverRegistered(const QString &objectName,
                                            Protocol::ObjectAddress objectAddress)
{
    if (objectName == m_objectName) {
        m_myAddress = objectAddress;
        connectToServer();
    }
}

void SelectionModelClient::serverUnregistered(const QString &objectName,
                                              Protocol::ObjectAddress objectAddress)
{
    Q_UNUSED(objectAddress);
    if (objectName == m_objectName)
        m_myAddress = Protocol::InvalidObjectAddress;
}
