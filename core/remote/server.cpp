#include "server.h"
#include "remotemodelserver.h"
#include "probe.h"

#include <network/protocol.h>
#include <network/message.h>

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
  if (isConnected()) {
    qDebug() << Q_FUNC_INFO << "connected already, refusing incoming connection.";
    m_tcpServer->nextPendingConnection()->close();
    return;
  }

  qDebug() << Q_FUNC_INFO;
  setDevice(m_tcpServer->nextPendingConnection());

  // send greeting message for protocol version check
  Message msg;
  msg.stream() << Protocol::ServerVersion << Protocol::version();
  stream() << msg;
}

void Server::messageReceived(const Message& msg)
{
  // ### temporary
  m_modelServer->newRequest(msg);
}

#include "server.moc"
