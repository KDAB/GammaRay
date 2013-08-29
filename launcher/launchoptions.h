#ifndef GAMMARAY_LAUNCHOPTIONS_H
#define GAMMARAY_LAUNCHOPTIONS_H

#include <QStringList>
#include <QHash>

class QVariant;

namespace GammaRay {

/** Describes the injection and probe options used for launching/attacing to a host process. */
class LaunchOptions
{
public:
  LaunchOptions();
  ~LaunchOptions();

  /** Returns @c true if this is valid and has launch arguments set. */
  bool isLaunch() const;

  /** Returns @c true if we are supposed to attach rather than start a new process. */
  bool isAttach() const;

  /** Returns @c true if no valid launch arguments or process id are set. */
  bool isValid() const;

  /** Call before launching to communicate probe settings. */
  void sendProbeSettings();

  /** Generic key/value settings send to the probe. */
  void setProbeSetting(const QString &key, const QVariant &value);

  /** Program and command line arguments to launch. */
  void setLaunchArguments(const QStringList &args);
  QStringList launchArguments() const;

  /** Process id for the process to attach to. */
  void setPid(int pid);
  int pid() const;

  /** In-process UI setting. */
  void setUseInProcessUi(bool enable);

private:
  QStringList m_launchArguments;
  int m_pid;
  QHash<QByteArray, QByteArray> m_probeSettings;
};
}

#endif // GAMMARAY_LAUNCHOPTIONS_H
