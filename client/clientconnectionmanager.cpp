/*
  clientconnectionmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "clientconnectionmanager.h"

#include "client.h"
#include "remotemodel.h"
#include "selectionmodelclient.h"
#include "propertycontrollerclient.h"
#include "probecontrollerclient.h"
#include "paintanalyzerclient.h"
#include "remoteviewclient.h"
#include <toolmanagerclient.h>

#include <common/objectbroker.h>
#include <common/streamoperators.h>

#include <ui/mainwindow.h>
#include <ui/splashscreen.h>
#include <ui/clienttoolmanager.h>

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

using namespace GammaRay;

static QAbstractItemModel *modelFactory(const QString &name)
{
    return new RemoteModel(name, qApp);
}

static QItemSelectionModel *selectionModelFactory(QAbstractItemModel *model)
{
    return new SelectionModelClient(model->objectName() + ".selection", model, qApp);
}

static QObject *createPropertyController(const QString &name, QObject *parent)
{
    return new PropertyControllerClient(name, parent);
}

static QObject *createProbeController(const QString &name, QObject *parent)
{
    QObject *o = new ProbeControllerClient(parent);
    ObjectBroker::registerObject(name, o);
    return o;
}

static QObject *createToolManager(const QString &name, QObject *parent)
{
    QObject *o = new ToolManagerClient(parent);
    ObjectBroker::registerObject(name, o);
    return o;
}

static QObject *createPaintAnalyzerClient(const QString &name, QObject *parent)
{
    return new PaintAnalyzerClient(name, parent);
}

static QObject *createRemoteViewClient(const QString &name, QObject *parent)
{
    return new RemoteViewClient(name, parent);
}

void ClientConnectionManager::init()
{
    StreamOperators::registerOperators();

    ObjectBroker::registerClientObjectFactoryCallback<PropertyControllerInterface *>(
        createPropertyController);
    ObjectBroker::registerClientObjectFactoryCallback<ProbeControllerInterface *>(
        createProbeController);
    ObjectBroker::registerClientObjectFactoryCallback<ToolManagerInterface *>(createToolManager);
    ObjectBroker::registerClientObjectFactoryCallback<PaintAnalyzerInterface *>(
        createPaintAnalyzerClient);
    ObjectBroker::registerClientObjectFactoryCallback<RemoteViewInterface *>(createRemoteViewClient);

    ObjectBroker::setModelFactoryCallback(modelFactory);
    ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
}

ClientConnectionManager::ClientConnectionManager(QObject *parent, bool showSplashScreenOnStartUp)
    : QObject(parent)
    , m_client(new Client(this))
    , m_mainWindow(0)
    , m_ignorePersistentError(false)
    , m_tries(0)
{
    if (showSplashScreenOnStartUp)
        showSplashScreen();
    connect(m_client, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(m_client, SIGNAL(connectionEstablished()), SLOT(connectionEstablished()));
    connect(m_client, SIGNAL(transientConnectionError()), SLOT(transientConnectionError()));
    connect(m_client, SIGNAL(persisitentConnectionError(QString)),
            SIGNAL(persistentConnectionError(QString)));
    connect(this, SIGNAL(persistentConnectionError(QString)), SLOT(delayedHideSplashScreen()));
    connect(this, SIGNAL(ready()), this, SLOT(delayedHideSplashScreen()));
}

ClientConnectionManager::~ClientConnectionManager()
{
    delete m_mainWindow;
}

QMainWindow *ClientConnectionManager::mainWindow() const
{
    return m_mainWindow;
}

void ClientConnectionManager::connectToHost(const QUrl &url, int tryAgain)
{
    m_serverUrl = url;
    m_connectionTimeout.start();
    m_tries = tryAgain;
    connectToHost();
}

void ClientConnectionManager::disconnectFromHost()
{
    m_client->disconnectFromHost();
}

void ClientConnectionManager::connectToHost()
{
    m_client->connectToHost(m_serverUrl, m_tries ? m_tries-- : 0);
}

void ClientConnectionManager::connectionEstablished()
{
    auto toolManager = ClientToolManager::instance();
    if (!toolManager) {
        toolManager = new ClientToolManager(this);
    }

    // We cannot instanciate the tool manager in the ctor as it's too early (not yet connected) and will
    // cause object registering issues, so make sure we connect only once.
    connect(toolManager, SIGNAL(toolListAvailable()), this, SIGNAL(ready()), Qt::UniqueConnection);
    toolManager->requestAvailableTools();
}

QMainWindow *ClientConnectionManager::createMainWindow()
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

void ClientConnectionManager::handlePersistentConnectionError(const QString &msg)
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
    QTimer::singleShot(0, this, SLOT(hideSplashScreen()));
}

void ClientConnectionManager::hideSplashScreen()
{
    ::hideSplashScreen();
}

void ClientConnectionManager::targetQuitRequested()
{
    m_ignorePersistentError = true;
}
