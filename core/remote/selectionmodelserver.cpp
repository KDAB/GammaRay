#include "selectionmodelserver.h"
#include "server.h"

using namespace GammaRay;

SelectionModelServer::SelectionModelServer(const QString& objectName, QAbstractItemModel* model, QObject* parent):
  NetworkSelectionModel(objectName, model, parent)
{
  m_myAddress = Server::instance()->registerObject(objectName, this, "newMessage");
}

SelectionModelServer::~SelectionModelServer()
{
}


#include "selectionmodelserver.moc"
