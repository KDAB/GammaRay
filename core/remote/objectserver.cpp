#include "objectserver.h"
#include "server.h"

#include <network/objectbroker.h>

using namespace GammaRay;

ObjectServer::ObjectServer(const QString& objectName, QObject* parent) : NetworkObject(objectName, parent)
{
  m_myAddress = Server::instance()->registerObject(objectName, this, "newMessage");
  ObjectBroker::registerObject(objectName, this);
}

ObjectServer::~ObjectServer()
{
}

#include "objectserver.moc"
