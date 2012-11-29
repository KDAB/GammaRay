#include "client.h"
#include "remotemodel.h" // ### temporary

#include <network/message.h>

#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

Client::Client(QObject* parent): Endpoint(parent), m_model(0)
{
}

Client::~Client()
{
}

Client* Client::instance()
{
  return static_cast<Client*>(s_instance);
}

void Client::connectToHost()
{
  QTcpSocket *sock = new QTcpSocket(this);
  sock->connectToHost(QHostAddress::LocalHost, defaultPort());
  // TODO: make async
  sock->waitForConnected();

  setDevice(sock);
}

void Client::messageReceived(const Message& msg)
{
  // TODO ServerVersion is the very first message we get!
  if (msg.address() == objectAddress(QLatin1String("com.kdab.GammaRay.Server"))) {
    switch (msg.type()) {
      case Protocol::ServerVersion:
      {
        qint32 serverVersion;
        msg.stream() >> serverVersion;
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
        msg.stream() >> name >> addr;
        qDebug() << Q_FUNC_INFO << "ObjectAdded" << name << addr;
        registerObjectInternal(name, addr);
        break;
      }
      case Protocol::ObjectRemoved:
      {
        QString name;
        msg.stream() >> name;
        unregisterObjectInternal(name);
        break;
      }
      case Protocol::ObjectMapReply:
      {
        msg.stream() >> m_objectsAddresses;
        qDebug() << Q_FUNC_INFO << "ObjectMapReply" << m_objectsAddresses;
      }
      default:
        return;
    }
  }

  // ### temporary
  if (m_model)
    m_model->newMessage(msg);
}

void Client::setModel(RemoteModel* model)
{
  m_model = model;
}

#include "client.moc"
