#ifndef GAMMARAY_OBJECTCLIENT_H
#define GAMMARAY_OBJECTCLIENT_H

#include <network/networkobject.h>

namespace GammaRay {

/** Client-side wrapper for NetworkObject instances.
 *  Never create manualy but retrieve via GammaRay::ObjectBroker.
 */
class ObjectClient : public NetworkObject
{
  Q_OBJECT
public:
  explicit ObjectClient(const QString& objectName, QObject* parent);
  ~ObjectClient();

private slots:
  void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void serverunRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

private:
  void connectToServer();
};

}

#endif // GAMMARAY_OBJECTCLIENT_H
