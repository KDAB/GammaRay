#include "objectserver.h"
#include "server.h"

using namespace GammaRay;

ObjectServer::ObjectServer(const QString& objectName, QObject* parent) : NetworkObject(objectName, parent)
{
  m_myAddress = Server::instance()->registerObject(objectName, this, "newMessage");
}

ObjectServer::~ObjectServer()
{
}

#include "objectserver.moc"
