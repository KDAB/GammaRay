#include "selectionmodelclient.h"
#include "client.h"

using namespace GammaRay;

SelectionModelClient::SelectionModelClient(const QString& objectName, QAbstractItemModel* model, QObject* parent) :
  NetworkSelectionModel(objectName, model, parent)
{
  m_myAddress = Client::instance()->objectAddress(objectName);
  connect(Client::instance(), SIGNAL(objectRegistered(QString,Protocol::ObjectAddress)), SLOT(serverRegistered(QString,Protocol::ObjectAddress)));
  connect(Client::instance(), SIGNAL(objectUnregistered(QString,Protocol::ObjectAddress)),  SLOT(serverUnregistered(QString,Protocol::ObjectAddress)));
  connectToServer();
}

SelectionModelClient::~SelectionModelClient()
{
}

void SelectionModelClient::connectToServer()
{
  if (m_myAddress == Protocol::InvalidObjectAddress)
    return;
  Client::instance()->registerForObject(m_myAddress, this, "newMessage");
  // TODO: send initial selection?
}

void SelectionModelClient::serverRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  if (objectName == m_objectName) {
    m_myAddress = objectAddress;
    connectToServer();
  }
}

void SelectionModelClient::serverunRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  Q_UNUSED(objectAddress);
  if (objectName == m_objectName)
    m_myAddress = Protocol::InvalidObjectAddress;
}

#include "selectionmodelclient.moc"
