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

  QString hostName = QLatin1String("localhost");
  quint16 port = Client::defaultPort();

  if (app.arguments().size() == 2) {
    hostName = app.arguments().at(1);
  } else if (app.arguments().size() == 3) {
    hostName = app.arguments().at(1);
    port = app.arguments().at(2).toUShort();
  }

  Client client;
  client.connectToHost(hostName, port);
  QObject::connect(&client, SIGNAL(disconnected()), &app, SLOT(quit()));

  RemoteModel model(QLatin1String("com.kdab.GammaRay.ObjectTree"));
  QTreeView view;
  view.setModel(&model);
  view.show();

  RemoteModel model2(QLatin1String("com.kdab.GammaRay.StaticPropertyModel"));
  QTreeView view2;
  view2.setModel(&model2);
  view2.show();

  return app.exec();
}
