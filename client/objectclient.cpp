#include "objectclient.h"
#include "client.h"

using namespace GammaRay;

ObjectClient::ObjectClient(const QString& objectName, QObject* parent) : NetworkObject(objectName, parent)
{
  m_myAddress = Client::instance()->objectAddress(objectName);
  connect(Client::instance(), SIGNAL(objectRegistered(QString,Protocol::ObjectAddress)), SLOT(serverRegistered(QString,Protocol::ObjectAddress)));
  connect(Client::instance(), SIGNAL(objectUnregistered(QString,Protocol::ObjectAddress)),  SLOT(serverUnregistered(QString,Protocol::ObjectAddress)));
  connectToServer();
}

ObjectClient::~ObjectClient()
{
}

void ObjectClient::connectToServer()
{
  if (m_myAddress == Protocol::InvalidObjectAddress)
    return;
  Client::instance()->registerForObject(m_myAddress, this, "newMessage");
}

void ObjectClient::serverRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  if (objectName == m_objectName) {
    m_myAddress = objectAddress;
    connectToServer();
  }
}

void ObjectClient::serverunRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  Q_UNUSED(objectAddress);
  if (objectName == m_objectName)
    m_myAddress = Protocol::InvalidObjectAddress;
}

#include "objectclient.moc"
