#ifndef GAMMARAY_SELECTIONMODELCLIENT_H
#define GAMMARAY_SELECTIONMODELCLIENT_H

#include <network/networkselectionmodel.h>

namespace GammaRay {

/** Client side of the network transparent QItemSelectionModel. */
class SelectionModelClient : public NetworkSelectionModel
{
  Q_OBJECT
public:
  SelectionModelClient(const QString& objectName, QAbstractItemModel* model, QObject* parent);
  ~SelectionModelClient();

private slots:
  void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void serverunRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

private:
  void connectToServer();
};

}

#endif // GAMMARAY_SELECTIONMODELCLIENT_H
