#ifndef GAMMARAY_OBJECTSERVER_H
#define GAMMARAY_OBJECTSERVER_H

#include <network/networkobject.h>

namespace GammaRay {

/** Server-side part of NetworkObject. */
class ObjectServer : public GammaRay::NetworkObject
{
  Q_OBJECT
public:
  explicit ObjectServer(const QString& objectName, QObject* parent);
  ~ObjectServer();
};

}

#endif // GAMMARAY_OBJECTSERVER_H
