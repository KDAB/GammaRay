/*
  launcher.h

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

#ifndef GAMMARAY_LAUNCHER_H
#define GAMMARAY_LAUNCHER_H

#include <QObject>
#include <QTimer>
#include "gammaray_launcher_export.h"

class QSharedMemory;
class QProcessEnvironment;

namespace GammaRay {

class LaunchOptions;
struct LauncherPrivate;
/** The actual launcher logic of gammaray.exe. */
class GAMMARAY_LAUNCHER_EXPORT Launcher : public QObject
{
  Q_OBJECT
public:
  explicit Launcher(const LaunchOptions &options, QObject *parent = 0);
  ~Launcher();

  /** This is used to identify the communication channels used by the launcher and the target process. */
  qint64 instanceIdentifier() const;
  bool start();
  void stop();
  void setProcessEnvironment(const QProcessEnvironment &env);

signals:
  void started();
  void finished();

protected:
  virtual void startClient(const QUrl &serverAddress);

private slots:
  void semaphoreReleased();
  void injectorError(int exitCode, const QString &errorMessage);
  void injectorFinished();
  void timeout();

private:
  void sendLauncherId();
  void sendProbeSettings();
  // in case shared memory isn't available
  void sendProbeSettingsFallback();
  void checkDone();

private:
  Q_DISABLE_COPY(Launcher)
  LauncherPrivate *p;
};
}

#endif // GAMMARAY_LAUNCHER_H
