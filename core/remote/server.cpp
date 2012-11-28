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
  m_tcpServer(new QTcpServer(this)),
  m_nextAddress(Protocol::InvalidObjectAddress)
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

  // TODO send complete object map
}

void Server::messageReceived(const Message& msg)
{
  // ### temporary
  m_modelServer->newRequest(msg);
}

Protocol::ObjectAddress Server::registerObject(const QString& objectName, QObject* receiver, const char* messageHandlerName)
{
  registerObjectInternal(objectName, ++m_nextAddress);
  Q_ASSERT(m_nextAddress);
  m_messageHandlers.insert(m_nextAddress, qMakePair<QObject*, QByteArray>(receiver, messageHandlerName));
  m_objectToNameMap.insert(receiver, objectName);
  registerObjectInternal(objectName, m_nextAddress);
  connect(receiver, SIGNAL(destroyed(QObject*)), SLOT(objectDestroyed(QObject*)));

  // TODO send notification message

  return m_nextAddress;
}

void Server::objectDestroyed(QObject* object)
{
  const QString objectName = m_objectToNameMap.value(object);
  m_objectToNameMap.remove(object);
  const Protocol::ObjectAddress addr = objectAddress(objectName);
  m_messageHandlers.remove(addr);
  unregisterObjectInternal(objectName);
}


#include "server.moc"
