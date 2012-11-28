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
  if (msg.type() == Protocol::ServerVersion) {
    qint32 serverVersion;
    msg.stream() >> serverVersion;
    if (serverVersion != Protocol::version()) {
      qCritical() << "Server version is" << serverVersion << ", was expecting" << Protocol::version() << " - aborting";
      exit(1);
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
