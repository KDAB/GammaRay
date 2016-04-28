/*
  launcher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "launcher.h"
#include "probefinder.h"
#include "probeabi.h"
#include "clientlauncher.h"
#include "launchoptions.h"
#include "injector/abstractinjector.h"
#include "injector/injectorfactory.h"

#include <common/endpoint.h>
#include <common/message.h>

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QUrl>

#include <iostream>

namespace GammaRay {

enum State {
  Initial = 0,
  InjectorFinished = 1,
  InjectorFailed = 2,
  ClientStarted = 4,
  Complete = InjectorFinished | ClientStarted
};

struct LauncherPrivate
{
  LauncherPrivate(const LaunchOptions& options) :
    options(options),
    server(Q_NULLPTR),
    socket(Q_NULLPTR),
    state(Initial),
    exitCode(0)
  {}

  AbstractInjector::Ptr createInjector() const
  {
    if (options.injectorType().isEmpty()) {
      if (options.isAttach()) {
        return InjectorFactory::defaultInjectorForAttach();
      } else {
        return InjectorFactory::defaultInjectorForLaunch(options.probeABI());
      }
    }
    return InjectorFactory::createInjector(options.injectorType(), options.injectorTypeExecutableOverride());
  }

  LaunchOptions options;
  QLocalServer *server;
  QLocalSocket *socket;
  ClientLauncher client;
  QTimer safetyTimer;
  AbstractInjector::Ptr injector;
  QUrl serverAddress;
  QString errorMessage;
  int state;
  int exitCode;
};

}

using namespace GammaRay;

Launcher::Launcher(const LaunchOptions& options, QObject* parent):
  QObject(parent),
  d(new LauncherPrivate(options))
{
  Q_ASSERT(options.isValid());

  const auto timeout = qgetenv("GAMMARAY_LAUNCHER_TIMEOUT").toInt();
  d->safetyTimer.setInterval(std::max(60, timeout) * 1000);
  d->safetyTimer.setSingleShot(true);
  connect(&d->safetyTimer, SIGNAL(timeout()), SLOT(timeout()));
}

Launcher::~Launcher()
{
  stop();
  d->client.waitForFinished();
  delete d;
}

qint64 Launcher::instanceIdentifier() const
{
  if (d->options.isAttach())
    return d->options.pid();
  return QCoreApplication::applicationPid();
}

void Launcher::stop()
{
    d->injector->stop();
}

bool Launcher::start()
{
  auto probeDll = d->options.probePath();
  if (probeDll.isEmpty()) {
    probeDll = ProbeFinder::findProbe(d->options.probeABI());
    d->options.setProbePath(QFileInfo(probeDll).absolutePath());
  }

  sendLauncherId();
  setupProbeSettingsServer();

  if (d->options.uiMode() != LaunchOptions::InProcessUi) {
    d->safetyTimer.start();
  }
  d->injector = d->createInjector();
  if (!d->injector) {
    if (d->options.injectorType().isEmpty()) {
      if (d->options.isAttach()) {
        injectorError(-1, tr("Uh-oh, there is no default attach injector on this platform."));
      } else {
        injectorError(-1, tr("Uh-oh, there is no default launch injector on this platform."));
      }
    } else {
      injectorError(-1, tr("Injector %1 not found.").arg(d->options.injectorType()));
    }
    return false;
  }

  connect(d->injector.data(), SIGNAL(started()), this, SLOT(restartTimer()));
  connect(d->injector.data(), SIGNAL(finished()), this, SLOT(injectorFinished()), Qt::QueuedConnection);
  if (d->options.isLaunch())
      connect(d->injector.data(), SIGNAL(attached()), this, SLOT(injectorFinished()), Qt::QueuedConnection);
  connect(d->injector.data(), SIGNAL(stderrMessage(QString)), this, SIGNAL(stderrMessage(QString)));
  connect(d->injector.data(), SIGNAL(stdoutMessage(QString)), this, SIGNAL(stdoutMessage(QString)));

  bool success = false;
  if (d->options.isLaunch()) {
    d->injector->setWorkingDirectory(d->options.workingDirectory());
    success = d->injector->launch(d->options.launchArguments(), probeDll, QStringLiteral("gammaray_probe_inject"), d->options.processEnvironment());
  } else if (d->options.isAttach()) {
    success = d->injector->attach(d->options.pid(), probeDll, QStringLiteral("gammaray_probe_attach"));
  }

  if (!success) {
    QString errorMessage;
    if (d->options.isLaunch())
      errorMessage = tr("Failed to launch target '%1'.").arg(d->options.launchArguments().join(QStringLiteral(" ")));
    if (d->options.isAttach())
      errorMessage = tr("Failed to attach to target with PID %1.").arg(d->options.pid());
    if (!d->injector->errorString().isEmpty())
      errorMessage += tr("\nError: %1").arg(d->injector->errorString());
    injectorError(d->injector->exitCode() ? d->injector->exitCode() : 1, errorMessage);
    return false;
  }
  return true;
}

int Launcher::exitCode() const
{
  return d->exitCode;
}

QString Launcher::errorMessage() const
{
  return d->errorMessage;
}

QUrl Launcher::serverAddress() const
{
  return d->serverAddress;
}

void Launcher::sendLauncherId()
{
  // if we are launching a new process, make sure it knows how to talk to us
  if (d->options.isLaunch()) {
      d->options.setProbeSetting(QStringLiteral("LAUNCHER_ID"), instanceIdentifier());
  }
}

void Launcher::setupProbeSettingsServer()
{
    d->server = new QLocalServer(this);
    d->server->setMaxPendingConnections(1);
    connect(d->server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    const QString serverName = QStringLiteral("gammaray-") + QString::number(instanceIdentifier());
    d->server->removeServer(serverName);
    if (!d->server->listen(serverName))
        qWarning() << "Unable to send probe settings:" << d->server->errorString();
}

void Launcher::startClient(const QUrl& serverAddress)
{
  if (!d->client.launch(serverAddress)) {
    qCritical("Unable to launch gammaray-client!");
    QCoreApplication::exit(1);
  }
}

void Launcher::injectorFinished()
{
  d->exitCode = d->injector->exitCode();
  if (d->errorMessage.isEmpty()) {
    d->errorMessage = d->injector->errorString();
    if (!d->errorMessage.isEmpty()) {
      d->state |= InjectorFailed;
      std::cerr << "Injector error: " << qPrintable(d->errorMessage) << std::endl;
    }
  }

  if ((d->state & InjectorFailed) == 0)
    d->state |= InjectorFinished;
  checkDone();
}

void Launcher::injectorError(int exitCode, const QString& errorMessage)
{
  d->exitCode = exitCode;
  d->errorMessage = errorMessage;

  d->state |= InjectorFailed;
  std::cerr << qPrintable(errorMessage) << std::endl;
  std::cerr << "See <https://github.com/KDAB/GammaRay/wiki/Known-Issues> for troubleshooting" <<  std::endl;
  checkDone();
}

void Launcher::timeout()
{
  d->state |= InjectorFailed;

  std::cerr << "Target not responding - timeout." << std::endl;
  std::cerr << "See <https://github.com/KDAB/GammaRay/wiki/Known-Issues> for troubleshooting" <<  std::endl;
  checkDone();
}

void Launcher::restartTimer()
{
    d->safetyTimer.stop();
    d->safetyTimer.start();
}


void Launcher::checkDone()
{
  if (d->state == Complete || (d->options.uiMode() == LaunchOptions::InProcessUi && d->state == InjectorFinished)) {
    emit finished();
  }
  else if ((d->state & InjectorFailed) != 0) {
    d->client.terminate();
    if (d->exitCode == 0)
      d->exitCode = 1;
    emit finished();
  }
}

void Launcher::newConnection()
{
    if (d->socket)
        return;
    d->socket = d->server->nextPendingConnection();
    connect(d->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    {
        Message msg(Protocol::LauncherAddress, Protocol::ServerVersion);
        msg.payload() << Protocol::version();
        msg.write(d->socket);
    }

    {
        Message msg(Protocol::LauncherAddress, Protocol::ProbeSettings);
        msg.payload() << d->options.probeSettings();
        msg.write(d->socket);
    }
}

void Launcher::readyRead()
{
    while (Message::canReadMessage(d->socket)) {
        const auto msg = Message::readMessage(d->socket);
        switch (msg.type()) {
            case Protocol::ServerAddress:
            {
                msg.payload() >> d->serverAddress;
                break;
            }
            default:
                continue;
        }
    }

    if (d->serverAddress.isEmpty())
        return;

    d->safetyTimer.stop();
    std::cout << "GammaRay server listening on: " << qPrintable(d->serverAddress.toString()) << std::endl;

    if (d->options.uiMode() == LaunchOptions::OutOfProcessUi) {
        startClient(d->serverAddress);
    }

    if (d->options.isAttach())
        emit attached();

    d->state |= ClientStarted;
    checkDone();
    emit started();
}
