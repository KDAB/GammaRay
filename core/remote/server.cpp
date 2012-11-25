#include "server.h"
#include "remotemodelserver.h"
#include "probe.h"

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

using namespace GammaRay;

Server::Server(QObject *parent) : 
  Endpoint(parent),
  m_tcpServer(new QTcpServer(this))
{
  connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
  m_tcpServer->listen(QHostAddress::Any, defaultPort());

  // ### temporary, proof of concept
  m_modelServer = new RemoteModelServer(this);
  m_modelServer->setModel(Probe::instance()->objectTreeModel());
}

Server::~Server()
{
}

void Server::newConnection()
{
  qDebug() << Q_FUNC_INFO;
  // TODO: deal with more than one connection request

  setDevice(m_tcpServer->nextPendingConnection());

  // ### temporary
  connect(this, SIGNAL(messageReceived(GammaRay::Message)), m_modelServer, SLOT(newRequest(GammaRay::Message)));
}

#include "server.moc"
