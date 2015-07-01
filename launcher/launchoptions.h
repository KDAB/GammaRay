/*
  launchoptions.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_LAUNCHOPTIONS_H
#define GAMMARAY_LAUNCHOPTIONS_H

#include <QHash>
#include <QStringList>

#include "gammaray_launcher_export.h"

class QVariant;

namespace GammaRay {

class LaunchOptionsPrivate;
class ProbeABI;
/** Describes the injection and probe options used for launching/attacing to a host process. */
class GAMMARAY_LAUNCHER_EXPORT LaunchOptions
{
public:
  LaunchOptions();
  ~LaunchOptions();

  enum UiMode {
    InProcessUi,
    OutOfProcessUi,
    NoUi
  };

  /** Returns @c true if this is valid and has launch arguments set. */
  bool isLaunch() const;

  /** Returns @c true if we are supposed to attach rather than start a new process. */
  bool isAttach() const;

  /** Returns @c true if no valid launch arguments or process id are set. */
  bool isValid() const;

  void setProbePath(const QString &path);
  const QString &probePath() const;

  /** Generic key/value settings send to the probe. */
  void setProbeSetting(const QString &key, const QVariant &value);
  QHash<QByteArray, QByteArray> probeSettings() const;

  /** Program and command line arguments to launch. */
  void setLaunchArguments(const QStringList &args);
  QStringList launchArguments() const;
  /** Absolute path (as far as it can be determined) of the executable to launch.
   *  Only valid if isLaunch() returns @c true.
   */
  QString absoluteExecutablePath() const;

  /** Process id for the process to attach to. */
  void setPid(int pid);
  int pid() const;

  /** UI mode. */
  UiMode uiMode() const;
  void setUiMode(UiMode mode);

  /** Injector type. */
  QString injectorType() const;
  void setInjectorType(const QString &injectorType);

  /** Probe ABI. */
  const ProbeABI& probeABI() const;
  void setProbeABI(const ProbeABI &abi);

  /** execute this launch options with the given command-line launcher. */
  bool execute(const QString& launcherPath) const;

  LaunchOptions(const LaunchOptions &other);
  LaunchOptions &operator=(const LaunchOptions &);
private:
  LaunchOptionsPrivate *p;
};
}

#endif // GAMMARAY_LAUNCHOPTIONS_H
