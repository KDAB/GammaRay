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
  QTreeView view;
  view.setModel(&model);
  view.show();

  RemoteModel model2(QLatin1String("com.kdab.GammaRay.ObjectList"));
  QTreeView view2;
  view2.setModel(&model2);
  view2.show();

  return app.exec();
}
