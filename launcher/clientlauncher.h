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

  bool launch(const QString &hostName, quint16 port = 0);
  void terminate();
  void waitForFinished();

  static void launchDetached(const QString &hostName, quint16 port = 0);

private:
  static QString clientPath();
  static QStringList makeArgs(const QString &hostName, quint16 port);

private:
  QProcess m_process;
};
}

#endif // GAMMARAY_CLIENTLAUNCHER_H
