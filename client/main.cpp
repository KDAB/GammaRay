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
  QObject::connect(&client, SIGNAL(disconnected()), &app, SLOT(quit()));

  RemoteModel model(QLatin1String("com.kdab.GammaRay.ObjectTree"));
  client.setModel(&model);
  QTreeView view;
  view.setModel(&model);

  view.show();
  return app.exec();
}
