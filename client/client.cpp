#include "client.h"

#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

Client::Client(QObject* parent): Endpoint(parent)
{
}

Client::~Client()
{
}

void Client::connectToHost()
{
  QTcpSocket *sock = new QTcpSocket(this);
  sock->connectToHost(QHostAddress::LocalHost, defaultPort());
  // TODO: make async, send handshake message to verify compatible versions
  sock->waitForConnected();

  setDevice(sock);
}


#include "client.moc"
