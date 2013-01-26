#include "client.h"

#include <network/message.h>

#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

Client::Client(QObject* parent): Endpoint(parent)
{
}

Client::~Client()
{
}

Client* Client::instance()
{
  return static_cast<Client*>(s_instance);
}

void Client::connectToHost(const QString &hostName, quint16 port)
{
  qDebug() << Q_FUNC_INFO << hostName << port;
  QTcpSocket *sock = new QTcpSocket(this);
  sock->connectToHost(hostName, port);
  // TODO: make async
  sock->waitForConnected();

  setDevice(sock);
}

void Client::messageReceived(const Message& msg)
{
  // TODO ServerVersion is the very first message we get!
  if (msg.address() == endpointAddress()) {
    switch (msg.type()) {
      case Protocol::ServerVersion:
      {
        qint32 serverVersion;
        msg.payload() >> serverVersion;
        if (serverVersion != Protocol::version()) {
          qCritical() << "Server version is" << serverVersion << ", was expecting" << Protocol::version() << " - aborting";
          exit(1);
        }
        break;
      }
      case Protocol::ObjectAdded:
      {
        QString name;
        Protocol::ObjectAddress addr;
        msg.payload() >> name >> addr;
        qDebug() << Q_FUNC_INFO << "ObjectAdded" << name << addr;
        registerObjectInternal(name, addr);
        break;
      }
      case Protocol::ObjectRemoved:
      {
        QString name;
        msg.payload() >> name;
        unregisterObjectInternal(name);
        break;
      }
      case Protocol::ObjectMapReply:
      {
        QVector<QPair<Protocol::ObjectAddress, QString> > objects;
        msg.payload() >> objects;
        for (QVector<QPair<Protocol::ObjectAddress, QString> >::const_iterator it = objects.constBegin(); it != objects.constEnd(); ++it) {
          if (it->first != endpointAddress())
            registerObjectInternal(it->second, it->first);
        }
        qDebug() << Q_FUNC_INFO << "ObjectMapReply" << objectAddresses();
      }
      default:
        return;
    }
  }

  dispatchMessage(msg);
}

void Client::registerForObject(Protocol::ObjectAddress objectAddress, QObject* handler, const char* slot)
{
  Q_ASSERT(isConnected());
  registerMessageHandlerInternal(objectAddress, handler, slot);
  Message msg(endpointAddress(), Protocol::ObjectMonitored);
  msg.payload() << objectAddress;
  send(msg);
}

void Client::unregisterForObject(Protocol::ObjectAddress objectAddress)
{
  unregisterMessageHandlerInternal(objectAddress);
  unmonitorObject(objectAddress);
}

void Client::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName)
{
  Q_UNUSED(objectName);
  unmonitorObject(objectAddress);
}

void Client::unmonitorObject(Protocol::ObjectAddress objectAddress)
{
  if (!isConnected())
    return;
  Message msg(endpointAddress(), Protocol::ObjectUnmonitored);
  msg.payload() << objectAddress;
  send(msg);
}

#include "client.moc"
