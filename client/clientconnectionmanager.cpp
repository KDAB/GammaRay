/*
  clientconnectionmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "enumrepositoryclient.h"
#include "classesiconsrepositoryclient.h"
#include "remotemodel.h"
#include "selectionmodelclient.h"
#include "propertycontrollerclient.h"
#include "probecontrollerclient.h"
#include "processtracker.h"
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

static QObject *createEnumRepositoryClient(const QString &, QObject *parent)
{
    return new EnumRepositoryClient(parent);
}

static QObject *createClassesIconsRepositoryClient(const QString &, QObject *parent)
{
    return new ClassesIconsRepositoryClient(parent);
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
    ObjectBroker::registerClientObjectFactoryCallback<EnumRepository*>(createEnumRepositoryClient);
    ObjectBroker::registerClientObjectFactoryCallback<ClassesIconsRepository*>(createClassesIconsRepositoryClient);

    ObjectBroker::setModelFactoryCallback(modelFactory);
    ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
}

ClientConnectionManager::ClientConnectionManager(QObject *parent, bool showSplashScreenOnStartUp)
    : QObject(parent)
    , m_client(new Client(this))
    , m_processTracker(new GammaRay::ProcessTracker(this))
    , m_toolManager(new ClientToolManager(this))
    , m_mainWindow(nullptr)
    , m_ignorePersistentError(false)
    , m_tries(0)
{
    if (showSplashScreenOnStartUp)
        showSplashScreen();
    connect(m_processTracker, &ProcessTracker::backendChanged, this,
            &ClientConnectionManager::processTrackerBackendChanged);
    connect(m_processTracker, &ProcessTracker::infoChanged, this,
            &ClientConnectionManager::processTrackerInfoChanged);
    connect(this, &ClientConnectionManager::ready, this, &ClientConnectionManager::clientConnected);
    connect(this, &ClientConnectionManager::disconnected, this, &ClientConnectionManager::clientDisconnected);
    connect(m_client, &Endpoint::disconnected, this, &ClientConnectionManager::disconnected);
    connect(m_client, &Client::transientConnectionError, this, &ClientConnectionManager::transientConnectionError);
    connect(m_client, &Client::persisitentConnectionError,
            this, &ClientConnectionManager::persistentConnectionError);
    connect(this, &ClientConnectionManager::persistentConnectionError, this, &ClientConnectionManager::delayedHideSplashScreen);
    connect(this, &ClientConnectionManager::ready, this, &ClientConnectionManager::delayedHideSplashScreen);
    connect(m_toolManager, &ClientToolManager::toolListAvailable, this, &ClientConnectionManager::ready);
}

ClientConnectionManager::~ClientConnectionManager()
{
    delete m_mainWindow;
}

ClientToolManager *ClientConnectionManager::toolManager() const
{
    return m_toolManager;
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
    doConnectToHost();
}

void ClientConnectionManager::showSplashScreen()
{
    ::showSplashScreen();
}

GammaRay::ProcessTrackerBackend *ClientConnectionManager::processTrackerBackend() const
{
    return m_processTracker->backend();
}

void ClientConnectionManager::setProcessTrackerBackend(GammaRay::ProcessTrackerBackend *backend)
{
    m_processTracker->setBackend(backend);
    updateProcessTrackerState();
}

qint64 ClientConnectionManager::processTrackerPid() const
{
    return m_processTracker->pid();
}

void ClientConnectionManager::setProcessTrackerPid(qint64 pid)
{
    m_processTracker->setPid(pid);
    updateProcessTrackerState();
}

QString ClientConnectionManager::endPointLabel() const
{
    return m_client->label();
}

QString ClientConnectionManager::endPointKey() const
{
    return m_client->key();
}

qint64 ClientConnectionManager::endPointPid() const
{
    return m_client->pid();
}

void ClientConnectionManager::disconnectFromHost()
{
    targetQuitRequested();
    m_client->disconnectFromHost();
}

void ClientConnectionManager::doConnectToHost()
{
    m_client->connectToHost(m_serverUrl, m_tries ? m_tries-- : 0);
}

QMainWindow *ClientConnectionManager::createMainWindow()
{
    delete m_mainWindow;
    m_mainWindow = new MainWindow;
    m_mainWindow->setupFeedbackProvider();
    connect(m_mainWindow.data(), &MainWindow::targetQuitRequested, this, &ClientConnectionManager::targetQuitRequested);
    m_ignorePersistentError = false;
    m_mainWindow->show();
    return m_mainWindow;
}

void ClientConnectionManager::transientConnectionError()
{
    if (m_connectionTimeout.elapsed() < 60 * 1000) {
        // client wasn't up yet, keep trying
        QTimer::singleShot(1000, this, &ClientConnectionManager::doConnectToHost);
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
    QTimer::singleShot(0, this, &ClientConnectionManager::hideSplashScreen);
}

void ClientConnectionManager::hideSplashScreen()
{
    ::hideSplashScreen();
}

void ClientConnectionManager::targetQuitRequested()
{
    m_ignorePersistentError = true;
}

void ClientConnectionManager::updateProcessTrackerState()
{
    if (!m_client->isConnected()) {
        m_processTracker->stop();
    }
    else if (m_processTracker->isActive()) {
        if (!m_processTracker->backend() || m_processTracker->pid() < 0) {
            m_processTracker->stop();
        }
    }
    else {
        if (m_processTracker->backend() && m_processTracker->pid() >= 0) {
            m_processTracker->start();
        }
    }
}

void ClientConnectionManager::clientConnected()
{
    setProcessTrackerPid(m_client->pid());
}

void ClientConnectionManager::clientDisconnected()
{
    setProcessTrackerPid(-1);
    emit processTrackerInfoChanged(ProcessTrackerInfo());
}
