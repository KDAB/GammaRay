#ifndef GAMMARAY_PROBECONTROLLERCLIENT_H
#define GAMMARAY_PROBECONTROLLERCLIENT_H

#include <common/probecontrollerinterface.h>

namespace GammaRay {

class ProbeControllerClient : public QObject, public GammaRay::ProbeControllerInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ProbeControllerInterface)
public:
  explicit ProbeControllerClient(QObject *parent = 0);
  void detachProbe();
  void quitHost();
};

}

#endif // GAMMARAY_PROBECONTROLLERCLIENT_H
