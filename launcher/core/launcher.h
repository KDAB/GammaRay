/*
  launcher.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "gammaray_launcher_export.h"

#include <QObject>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QProcessEnvironment;
class QUrl;
QT_END_NAMESPACE

namespace GammaRay {
class LaunchOptions;
struct LauncherPrivate;

/*! Manages launching a target process, injecting the probe and if needed also starting the client application. */
class GAMMARAY_LAUNCHER_EXPORT Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(const LaunchOptions &options, QObject *parent = nullptr);
    ~Launcher() override;

    /*! This is used to identify the communication channels used by the launcher and the target process. */
    qint64 instanceIdentifier() const;
    bool start();
    void stop();

    /*! Target exit code, in case we launched it. */
    int exitCode() const;
    /*! Error message from attaching/launching the target, if any. */
    QString errorMessage() const;

    /*! Address for the client to connect to, valid once received from the target. */
    QUrl serverAddress() const;
signals:
    void started();
    void finished();
    void attached();

    void stdoutMessage(const QString &message);
    void stderrMessage(const QString &message);

protected:
    virtual void startClient(const QUrl &serverAddress);

private slots:
    void injectorError(int exitCode, const QString &errorMessage);
    void injectorFinished();
    void timeout();
    void restartTimer();

    void newConnection();
    void readyRead();

private:
    void sendLauncherId();
    void setupProbeSettingsServer();
    void printAllAvailableIPs();
    void checkDone();

private:
    LauncherPrivate * const d;
};
}

#endif // GAMMARAY_LAUNCHER_H
