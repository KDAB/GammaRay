#include "remotemodel.h"

#include <QApplication>
#include <QTreeView>
#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // ### temporary, just for proof of concpet
  QTcpSocket sock;
  sock.connectToHost(QHostAddress::LocalHost, 11732);
  sock.waitForConnected();

  QDataStream stream(&sock);

  RemoteModel model;
  model.setStream(&stream);
  QObject::connect(&sock, SIGNAL(readyRead()), &model, SLOT(newMessage()));
  QTreeView view;
  view.setModel(&model);

  view.show();
  return app.exec();
}
