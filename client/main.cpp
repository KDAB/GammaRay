#include "remotemodel.h"
#include "client.h"
#include "selectionmodelclient.h"

#include <network/modelbroker.h>

#include <QApplication>
#include <QTreeView>

using namespace GammaRay;

static QAbstractItemModel* modelNotFoundCallback(const QString &name)
{
  return new RemoteModel(name, qApp);
}

static QItemSelectionModel* selectionModelNotFoundCallback(QAbstractItemModel* model)
{
  return new SelectionModelClient(model->objectName() + ".selection", model, qApp);
}

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

  // TODO make this async, show some status indicator/splash screen while connecting

  ModelBroker::setModelNotFoundCallback(modelNotFoundCallback);
  ModelBroker::setSelectionModelNotFoundCallback(selectionModelNotFoundCallback);

  QTreeView view;
  view.setModel(ModelBroker::model(QLatin1String("com.kdab.GammaRay.ObjectTree")));
  view.show();

  return app.exec();
}
