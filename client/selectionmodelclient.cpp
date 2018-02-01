/*
  selectionmodelclient.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(clearPendingSelection()));
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), m_timer, SLOT(start()));
    connect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), m_timer, SLOT(
                start()));
    connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)), m_timer, SLOT(start()));
    connect(model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), m_timer,
            SLOT(start()));
    connect(model, SIGNAL(layoutChanged()), m_timer, SLOT(start()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

    m_myAddress = Client::instance()->objectAddress(objectName);
    connect(Client::instance(), SIGNAL(objectRegistered(QString,Protocol::ObjectAddress)),
            SLOT(serverRegistered(QString,Protocol::ObjectAddress)));
    connect(Client::instance(), SIGNAL(objectUnregistered(QString,Protocol::ObjectAddress)),
            SLOT(serverUnregistered(QString,Protocol::ObjectAddress)));
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
    QTimer::singleShot(125, this, SLOT(requestSelection()));
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
