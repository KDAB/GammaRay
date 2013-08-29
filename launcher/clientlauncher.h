#ifndef GAMMARAY_CLIENTLAUNCHER_H
#define GAMMARAY_CLIENTLAUNCHER_H

#include <QProcess>

namespace GammaRay {

/** Launching/monitoring of the GammaRay client for out-of-process use. */
class ClientLauncher
{
public:
  ClientLauncher();
  ~ClientLauncher();

  bool launch(const QString &hostName); // TODO add port eventually
  void terminate();
  void waitForFinished();

private:
  QProcess m_process;
};
}

#endif // GAMMARAY_CLIENTLAUNCHER_H
