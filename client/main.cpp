#include "remotemodel.h"
#include "client.h"

#include <QApplication>
#include <QTreeView>
#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Client client;
  client.connectToHost();

  RemoteModel model;
  QObject::connect(&client, SIGNAL(messageReceived(GammaRay::Message)), &model, SLOT(newMessage(GammaRay::Message)));
  QTreeView view;
  view.setModel(&model);

  view.show();
  return app.exec();
}
