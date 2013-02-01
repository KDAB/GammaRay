#ifndef GAMMARAY_SELECTIONMODELSERVER_H
#define GAMMARAY_SELECTIONMODELSERVER_H

#include <network/networkselectionmodel.h>

namespace GammaRay {

/** Server-side of the network transparent QItemSelection model. */
class SelectionModelServer : public NetworkSelectionModel
{
  Q_OBJECT
public:
  explicit SelectionModelServer(const QString& objectName, QAbstractItemModel* model, QObject* parent);
  ~SelectionModelServer();
};

}

#endif // GAMMARAY_SELECTIONMODELSERVER_H
