/*
  clientconnectionmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "clientconnectionmanager.h"

#include "client.h"

#include <common/objectbroker.h>

#include <ui/mainwindow.h>
#include <ui/splashscreen.h>

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

using namespace GammaRay;

ClientConnectionManager::ClientConnectionManager(QObject* parent) :
  QObject(parent),
  m_port(0),
  m_client(new Client(this)),
  m_mainWindow(0),
  m_toolModel(0)
{
  showSplashScreen();

  connect(m_client, SIGNAL(disconnected()), QApplication::instance(), SLOT(quit()));
  connect(m_client, SIGNAL(connectionEstablished()), SLOT(connectionEstablished()));
  connect(m_client, SIGNAL(connectionError(QAbstractSocket::SocketError,QString)), SLOT(connectionError(QAbstractSocket::SocketError,QString)));
}

ClientConnectionManager::~ClientConnectionManager()
{
  delete m_mainWindow;
}

void ClientConnectionManager::connectToHost(const QString& hostname, quint16 port)
{
  m_hostname = hostname;
  m_port = port;
  m_connectionTimeout.start();
  connectToHost();
}

void ClientConnectionManager::connectToHost()
{
  m_client->connectToHost(m_hostname, m_port);
}

void ClientConnectionManager::connectionEstablished()
{
  m_toolModel = ObjectBroker::model("com.kdab.GammaRay.ToolModel");

  if (m_toolModel->rowCount() <= 0) {
    connect(m_toolModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(toolModelPopulated()));
    connect(m_toolModel, SIGNAL(layoutChanged()), SLOT(toolModelPopulated()));
    connect(m_toolModel, SIGNAL(modelReset()), SLOT(toolModelPopulated()));
  } else {
    toolModelPopulated();
  }
}

void ClientConnectionManager::toolModelPopulated()
{
  if (m_toolModel->rowCount() <= 0)
    return;

  disconnect(m_toolModel, 0, this, 0);

  m_mainWindow = new MainWindow;
  m_mainWindow->show();
  hideSplashScreen();
}

void ClientConnectionManager::connectionError(QAbstractSocket::SocketError error, const QString& msg)
{
  if (m_connectionTimeout.elapsed() < 60 * 1000 && error == QAbstractSocket::ConnectionRefusedError) {
    // client wasn't up yet, keep trying
    QTimer::singleShot(1000, this, SLOT(connectToHost()));
    return;
  }

  hideSplashScreen();

  QString errorMsg;
  if (m_mainWindow)
    errorMsg = tr("Lost connection to remote host: %1").arg(msg);
  else
    errorMsg = tr("Could not establish connection to remote host: %1").arg(msg);

  QMessageBox::critical(m_mainWindow, tr("GammaRay - Connection Error"), errorMsg);
  QApplication::exit(1);
}

