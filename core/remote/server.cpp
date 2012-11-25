#include "server.h"
#include "remotemodelserver.h"
#include "probe.h"

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

using namespace GammaRay;

Server::Server(QObject *parent) : 
  QObject(parent),
  m_tcpServer(new QTcpServer(this)),
  m_stream(0)
{
  connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
  m_tcpServer->listen(QHostAddress::Any, 11732);

  // ### temporary, proof of concept
  m_modelServer = new RemoteModelServer(this);
  m_modelServer->setModel(Probe::instance()->objectTreeModel());
}

Server::~Server()
{
  delete m_stream;
}

void Server::newConnection()
{
  qDebug() << Q_FUNC_INFO;

  QTcpSocket *socket = m_tcpServer->nextPendingConnection();
  m_stream = new QDataStream(socket);

  // ### temporary, proof of concept
  m_modelServer->setStream(m_stream);

  connect(socket, SIGNAL(readyRead()), m_modelServer, SLOT(newRequest()));
}

#include "server.moc"
