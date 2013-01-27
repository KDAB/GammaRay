#include "server.h"
#include "probe.h"

#include <network/protocol.h>
#include <network/message.h>

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkInterface>

using namespace GammaRay;

Server::Server(QObject *parent) : 
  Endpoint(parent),
  m_tcpServer(new QTcpServer(this)),
  m_nextAddress(endpointAddress()),
  m_broadcastTimer(new QTimer(this)),
  m_broadcastSocket(new QUdpSocket(this))
{
  connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
  m_tcpServer->listen(QHostAddress::Any, defaultPort());

  m_broadcastTimer->setInterval(5 * 1000);
  m_broadcastTimer->setSingleShot(false);
  m_broadcastTimer->start();
  connect(m_broadcastTimer, SIGNAL(timeout()), SLOT(broadcast()));
  connect(this, SIGNAL(disconnected()), m_broadcastTimer, SLOT(start()));
}

Server::~Server()
{
}

Server* Server::instance()
{
  Q_ASSERT(s_instance);
  return static_cast<Server*>(s_instance);
}


void Server::newConnection()
{
  if (isConnected()) {
    qDebug() << Q_FUNC_INFO << "connected already, refusing incoming connection.";
    m_tcpServer->nextPendingConnection()->close();
    return;
  }

  qDebug() << Q_FUNC_INFO;
  m_broadcastTimer->stop();
  setDevice(m_tcpServer->nextPendingConnection());

  // send greeting message for protocol version check
  {
    Message msg(endpointAddress(), Protocol::ServerVersion);
    msg.payload() << Protocol::version();
    send(msg);
  }

  {
    Message msg(endpointAddress(), Protocol::ObjectMapReply);
    msg.payload() << objectAddresses();
    send(msg);
  }
}

void Server::messageReceived(const Message& msg)
{
  if (msg.address() == endpointAddress()) {
    switch (msg.type()) {
      case Protocol::ObjectMonitored:
      case Protocol::ObjectUnmonitored:
      {
        Protocol::ObjectAddress addr;
        msg.payload() >> addr;
        Q_ASSERT(addr > Protocol::InvalidObjectAddress);
        const QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> >::const_iterator it = m_monitorNotifiers.constFind(addr);
        if (it == m_monitorNotifiers.constEnd())
          break;
        qDebug() << Q_FUNC_INFO << "un/monitor" << addr;
        QMetaObject::invokeMethod(it.value().first, it.value().second, Q_ARG(bool, msg.type() == Protocol::ObjectMonitored));
        break;
      }
    }
  } else {
    dispatchMessage(msg);
  }
}

Protocol::ObjectAddress Server::registerObject(const QString& objectName, QObject* receiver, const char* messageHandlerName, const char* monitorNotifier)
{
  registerObjectInternal(objectName, ++m_nextAddress);
  Q_ASSERT(m_nextAddress);
  registerMessageHandlerInternal(m_nextAddress, receiver, messageHandlerName);

  if (monitorNotifier)
    m_monitorNotifiers.insert(m_nextAddress, qMakePair<QObject*, QByteArray>(receiver, monitorNotifier));

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectAdded);
    msg.payload() <<  objectName << m_nextAddress;
    send(msg);
  }

  return m_nextAddress;
}

void Server::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName)
{
  unregisterObjectInternal(objectName);
  m_monitorNotifiers.remove(objectAddress);

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectRemoved);
    msg.payload() << objectName;
    send(msg);
  }
}

void Server::broadcast()
{
  QString myAddress;
  foreach (const QHostAddress &addr, QNetworkInterface::allAddresses()) {
    if (addr == QHostAddress::LocalHost || addr == QHostAddress::LocalHostIPv6)
      continue;
    myAddress = addr.toString();
    break;
  }

  QByteArray datagram;
  QDataStream stream(&datagram, QIODevice::WriteOnly);
  stream << Protocol::version();
  stream << myAddress;
  stream << defaultPort(); // might change for multiple instances on the same machine
  stream << m_label; // TODO integrate hostname
  m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, broadcastPort());
}

void Server::setLabel(const QString& label)
{
  m_label = label;
}

#include "server.moc"
