#include "clientconnectionmanager.h"

#include "client.h"
#include "clienttoolmodel.h"

#include <common/network/objectbroker.h>

#include <ui/mainwindow.h>
#include <ui/splashscreen.h>

#include <QApplication>
#include <QMessageBox>

using namespace GammaRay;

ClientConnectionManager::ClientConnectionManager(QObject* parent) :
  QObject(parent),
  m_client(new Client(this)),
  m_mainWindow(0)
{
  showSplashScreen();

  connect(m_client, SIGNAL(disconnected()), QApplication::instance(), SLOT(quit()));
  connect(m_client, SIGNAL(connectionEstablished()), SLOT(connectionEstablished()));
  connect(m_client, SIGNAL(connectionError(QString)), SLOT(connectionError(QString)));
}

ClientConnectionManager::~ClientConnectionManager()
{
}

void ClientConnectionManager::connectToHost(const QString& hostname, quint16 port)
{
    m_client->connectToHost(hostname, port);
}

void ClientConnectionManager::connectionEstablished()
{
  ClientToolModel *toolModel = new ClientToolModel(this);
  ObjectBroker::registerModel("com.kdab.GammaRay.ToolModel", toolModel);

  m_mainWindow = new MainWindow;
  m_mainWindow->show();
  hideSplashScreen();
}

void ClientConnectionManager::connectionError(const QString& msg)
{
  QMessageBox::critical(m_mainWindow, tr("Connection Error"), msg);
  QApplication::exit(1);
}

#include "clientconnectionmanager.moc"
