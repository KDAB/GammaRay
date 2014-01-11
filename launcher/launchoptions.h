/*
  launchoptions.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

class QVariant;

namespace GammaRay {

/** Describes the injection and probe options used for launching/attacing to a host process. */
class LaunchOptions
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

  /** Generic key/value settings send to the probe. */
  void setProbeSetting(const QString &key, const QVariant &value);
  QHash<QByteArray, QByteArray> probeSettings() const;

  /** Program and command line arguments to launch. */
  void setLaunchArguments(const QStringList &args);
  QStringList launchArguments() const;

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
  QString probeABI() const;
  void setProbeABI(const QString &abi);

  /** execute this launch options with the given command-line launcher. */
  bool execute(const QString& launcherPath) const;

private:
  QStringList m_launchArguments;
  QString m_injectorType;
  QString m_probeABI;
  int m_pid;
  UiMode m_uiMode;
  QHash<QByteArray, QByteArray> m_probeSettings;
};
}

#endif // GAMMARAY_LAUNCHOPTIONS_H
