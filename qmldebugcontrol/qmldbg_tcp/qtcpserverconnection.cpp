/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtcpserverconnection.h"

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

#include "private/qdeclarativedebugserver_p.h"
#include <private/qpacketprotocol_p.h>

QT_BEGIN_NAMESPACE

class QTcpServerConnectionPrivate {
public:
    QTcpServerConnectionPrivate();

    int port;
    QTcpSocket *socket;
    QPacketProtocol *protocol;
    QTcpServer *tcpServer;

    QDeclarativeDebugServer *debugServer;
};

QTcpServerConnectionPrivate::QTcpServerConnectionPrivate() :
    port(0),
    socket(0),
    protocol(0),
    tcpServer(0),
    debugServer(0)
{
}

QTcpServerConnection::QTcpServerConnection() :
    d_ptr(new QTcpServerConnectionPrivate)
{

}

QTcpServerConnection::~QTcpServerConnection()
{
    delete d_ptr;
}

void QTcpServerConnection::setServer(QDeclarativeDebugServer *server)
{
    Q_D(QTcpServerConnection);
    d->debugServer = server;
}

bool QTcpServerConnection::isConnected() const
{
    Q_D(const QTcpServerConnection);
    return d->socket && d->socket->state() == QTcpSocket::ConnectedState;
}

void QTcpServerConnection::send(const QByteArray &message)
{
    Q_D(QTcpServerConnection);

    if (!isConnected()
            || !d->protocol || !d->socket)
        return;

    QPacket pack;
    pack.writeRawData(message.data(), message.length());

    d->protocol->send(pack);
    d->socket->flush();
}

void QTcpServerConnection::disconnect()
{
    Q_D(QTcpServerConnection);

    // protocol might still be processing packages at this point
    d->protocol->deleteLater();
    d->protocol = 0;
    d->socket->deleteLater();
    d->socket = 0;
}

void QTcpServerConnection::setPort(int port, bool block)
{
    Q_D(QTcpServerConnection);
    d->port = port;

    listen();
    if (block)
        d->tcpServer->waitForNewConnection(-1);
}

void QTcpServerConnection::listen()
{
    Q_D(QTcpServerConnection);

    d->tcpServer = new QTcpServer(this);
    QObject::connect(d->tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if (d->tcpServer->listen(QHostAddress::Any, d->port))
        qWarning("QDeclarativeDebugServer: Waiting for connection on port %d...", d->port);
    else
        qWarning("QDeclarativeDebugServer: Unable to listen on port %d", d->port);
}


void QTcpServerConnection::readyRead()
{
    Q_D(QTcpServerConnection);
    if (!d->protocol)
        return;

    QPacket packet = d->protocol->read();

    QByteArray content = packet.data();
    d->debugServer->receiveMessage(content);
}

void QTcpServerConnection::newConnection()
{
    Q_D(QTcpServerConnection);

    if (d->socket) {
        qWarning("QDeclarativeDebugServer: Another client is already connected");
        QTcpSocket *faultyConnection = d->tcpServer->nextPendingConnection();
        delete faultyConnection;
        return;
    }

    d->socket = d->tcpServer->nextPendingConnection();
    d->socket->setParent(this);
    d->protocol = new QPacketProtocol(d->socket, this);
    QObject::connect(d->protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
}


Q_EXPORT_PLUGIN2(tcpserver, QTcpServerConnection)

QT_END_NAMESPACE

#include "qtcpserverconnection.moc"

