#ifndef GAMMARAY_LAUNCHER_H
#define GAMMARAY_LAUNCHER_H

#include <QObject>
#include <QTimer>

#include "launchoptions.h"
#include "clientlauncher.h"

class QSharedMemory;

namespace GammaRay {

/** The actual launcher logic of gammaray.exe. */
class Launcher : public QObject
{
  Q_OBJECT
public:
  explicit Launcher(const LaunchOptions &options, QObject *parent = 0);
  ~Launcher();

  /** This is used to identify the communication channels used by the launcher and the target process. */
  qint64 instanceIdentifier() const;

private slots:
  void delayedInit();
  void semaphoreReleased();
  void injectorError(int exitCode, const QString &errorMessage);
  void timeout();

private:
  void sendLauncherId();
  void sendProbeSettings();
  // in case shared memory isn't available
  void sendProbeSettingsFallback();

private:
  LaunchOptions m_options;
  QSharedMemory *m_shm;
  ClientLauncher m_client;
  QTimer m_safetyTimer;
};
}

#endif // GAMMARAY_LAUNCHER_H
