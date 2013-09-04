#ifndef GAMMARAY_PROBECONTROLLER_H
#define GAMMARAY_PROBECONTROLLER_H

#include <common/probecontrollerinterface.h>

namespace GammaRay {

class ProbeController : public QObject, public ProbeControllerInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ProbeControllerInterface)
public:
    explicit ProbeController(QObject *parent = 0);

public slots:
    void detachProbe();
    void quitHost();
};

}

#endif // GAMMARAY_PROBECONTROLLER_H
