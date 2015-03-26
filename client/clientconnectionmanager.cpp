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
#include "remotemodel.h"
#include "selectionmodelclient.h"
#include "propertycontrollerclient.h"
#include "probecontrollerclient.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>

#include <ui/mainwindow.h>
#include <ui/splashscreen.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

using namespace GammaRay;

static QAbstractItemModel* modelFactory(const QString &name)
{
  return new RemoteModel(name, qApp);
}

static QItemSelectionModel* selectionModelFactory(QAbstractItemModel* model)
{
  return new SelectionModelClient(model->objectName() + ".selection", model, qApp);
}

static QObject* createPropertyController(const QString &name, QObject *parent)
{
  return new PropertyControllerClient(name, parent);
}

static QObject* createProbeController(const QString &name, QObject *parent)
{
  QObject *o = new ProbeControllerClient(parent);
  ObjectBroker::registerObject(name, o);
  return o;
}

void ClientConnectionManager::init()
{
  StreamOperators::registerOperators();

  ObjectBroker::registerClientObjectFactoryCallback<PropertyControllerInterface*>(createPropertyController);
  ObjectBroker::registerClientObjectFactoryCallback<ProbeControllerInterface*>(createProbeController);
  ObjectBroker::setModelFactoryCallback(modelFactory);
  ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
}

ClientConnectionManager::ClientConnectionManager(QObject* parent) :
  QObject(parent),
  m_client(new Client(this)),
  m_mainWindow(0),
  m_toolModel(0),
  m_ignorePersistentError(false)
{
  showSplashScreen();

  connect(m_client, SIGNAL(disconnected()), SIGNAL(disconnected()));
  connect(m_client, SIGNAL(connectionEstablished()), SLOT(connectionEstablished()));
  connect(m_client, SIGNAL(transientConnectionError()), SLOT(transientConnectionError()));
  connect(m_client, SIGNAL(persisitentConnectionError(QString)), SIGNAL(persistentConnectionError(QString)));
  connect(this, SIGNAL(persistentConnectionError(QString)), SLOT(delayedHideSplashScreen()));
}

ClientConnectionManager::~ClientConnectionManager()
{
  delete m_mainWindow;
}

void ClientConnectionManager::connectToHost(const QUrl &url)
{
  m_serverUrl = url;
  m_connectionTimeout.start();
  connectToHost();
}

void ClientConnectionManager::connectToHost()
{
  m_client->connectToHost(m_serverUrl);
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
  QTimer::singleShot(0, this, SLOT(delayedHideSplashScreen()));
  emit ready();
}

QWidget *ClientConnectionManager::createMainWindow()
{
  delete m_mainWindow;
  m_mainWindow = new MainWindow;
  connect(m_mainWindow, SIGNAL(targetQuitRequested()), this, SLOT(targetQuitRequested()));
  m_ignorePersistentError = false;
  m_mainWindow->show();
  return m_mainWindow;
}

void ClientConnectionManager::transientConnectionError()
{
  if (m_connectionTimeout.elapsed() < 60 * 1000) {
    // client wasn't up yet, keep trying
    QTimer::singleShot(1000, this, SLOT(connectToHost()));
  } else {
    emit persistentConnectionError(tr("Connection refused."));
  }
}

void ClientConnectionManager::handlePersistentConnectionError(const QString& msg)
{
  if (m_ignorePersistentError)
    return;

  QString errorMsg;
  if (m_mainWindow)
    errorMsg = tr("Lost connection to remote host: %1").arg(msg);
  else
    errorMsg = tr("Could not establish connection to remote host: %1").arg(msg);

  QMessageBox::critical(m_mainWindow, tr("GammaRay - Connection Error"), errorMsg);
  QApplication::exit(1);
}

void ClientConnectionManager::delayedHideSplashScreen()
{
  hideSplashScreen();
}

void ClientConnectionManager::targetQuitRequested()
{
  m_ignorePersistentError = true;
}
