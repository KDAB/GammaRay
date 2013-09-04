#ifndef GAMMARAY_PROBECONTROLLERINTERFACE_H
#define GAMMARAY_PROBECONTROLLERINTERFACE_H

#include <QObject>

namespace GammaRay {

/** Probe and host process remote control functions. */
class ProbeControllerInterface
{
public:
  virtual ~ProbeControllerInterface();

  /** Terminate host application. */
  virtual void quitHost() = 0;

  /** Detach GammaRay but keep host application running. */
  virtual void detachProbe() = 0;
};

}

Q_DECLARE_INTERFACE(GammaRay::ProbeControllerInterface, "com.kdab.GammaRay.ProbeControllerInterface")

#endif // GAMMARAY_PROBECONTROLLERINTERFACE_H
