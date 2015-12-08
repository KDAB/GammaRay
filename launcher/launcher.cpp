/*
  launcher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "launcher.h"
#include "probefinder.h"
#include "probeabi.h"
#include "clientlauncher.h"
#include "launchoptions.h"
#include "injector/abstractinjector.h"
#include "injector/injectorfactory.h"

#include <common/endpoint.h>
#include <common/sharedmemorylocker.h>
#include <common/message.h>

#include <QByteArray>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QUrl>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QThread>

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
#ifdef HAVE_SHM
    shm(0),
#endif
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
    return InjectorFactory::createInjector(options.injectorType());
  }

  LaunchOptions options;
#ifdef HAVE_SHM
  QSharedMemory *shm;
#endif
  ClientLauncher client;
  QTimer safetyTimer;
  AbstractInjector::Ptr injector;
  QString errorMessage;
  int state;
  int exitCode;
};

}

using namespace GammaRay;

class SemaphoreWaiter : public QThread
{
  Q_OBJECT
public:
  explicit SemaphoreWaiter(qint64 id, QObject *parent = 0) : QThread(parent), m_id(id) {}
  ~SemaphoreWaiter() {}
  void run() Q_DECL_OVERRIDE
  {
#ifdef HAVE_SHM
    QSystemSemaphore sem("gammaray-semaphore-" + QString::number(m_id), 0, QSystemSemaphore::Create);
    sem.acquire();
    emit semaphoreReleased();
#endif
  }

signals:
  void semaphoreReleased();

private:
  qint64 m_id;
};

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
    probeDll = ProbeFinder::findProbe(QStringLiteral(GAMMARAY_PROBE_NAME), d->options.probeABI());
    d->options.setProbePath(QFileInfo(probeDll).absolutePath());
  }

  sendLauncherId();
  sendProbeSettings();

  if (d->options.uiMode() != LaunchOptions::InProcessUi) {
    SemaphoreWaiter *semWaiter = new SemaphoreWaiter(instanceIdentifier(), this);
    connect(semWaiter, SIGNAL(semaphoreReleased()), this, SLOT(semaphoreReleased()), Qt::QueuedConnection);
    semWaiter->start();

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

  connect(d->injector.data(), SIGNAL(started()), this, SIGNAL(started()));
  connect(d->injector.data(), SIGNAL(finished()), this, SLOT(injectorFinished()), Qt::QueuedConnection);
  connect(d->injector.data(), SIGNAL(attached()), this, SIGNAL(attached()), Qt::QueuedConnection);
  connect(d->injector.data(), SIGNAL(stderrMessage(QString)), this, SIGNAL(stderrMessage(QString)));
  connect(d->injector.data(), SIGNAL(stdoutMessage(QString)), this, SIGNAL(stdoutMessage(QString)));

  bool success = false;
  if (d->options.isLaunch()) {
    success = d->injector->launch(d->options.launchArguments(), probeDll, QStringLiteral("gammaray_probe_inject"), d->options.processEnvironment());
  } else if (d->options.isAttach()) {
    success = d->injector->attach(d->options.pid(), probeDll, QStringLiteral("gammaray_probe_inject"));
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

void Launcher::sendLauncherId()
{
  // if we are launching a new process, make sure it knows how to talk to us
  if (d->options.isLaunch()) {
      d->options.setProbeSetting(QStringLiteral("LAUNCHER_ID"), instanceIdentifier());
  }
}

void Launcher::sendProbeSettings()
{
#ifdef HAVE_SHM
  QByteArray ba; // need a full copy of this first, since there's no QIODevice to directly work on void*...
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);

  {
    Message msg(Protocol::LauncherAddress, Protocol::ServerVersion);
    msg.payload() << Protocol::version();
    msg.write(&buffer);
  }

  {
    Message msg(Protocol::LauncherAddress, Protocol::ProbeSettings);
    msg.payload() << d->options.probeSettings();
    msg.write(&buffer);
  }

  buffer.close();

  d->shm = new QSharedMemory(QStringLiteral("gammaray-") + QString::number(instanceIdentifier()), this);
  if (!d->shm->create(qMax(ba.size(), 1024))) { // make sure we have enough space for the answer
    qWarning() << Q_FUNC_INFO << "Failed to obtain shared memory for probe settings:" << d->shm->errorString()
      << "- error code (QSharedMemory::SharedMemoryError):" << d->shm->error();
    delete d->shm;
    d->shm = 0;
    return;
  }

  SharedMemoryLocker locker(d->shm);
  qMemCopy(d->shm->data(), ba.constData(), ba.size());
  if (d->shm->size() > ba.size()) // Windows...
    qMemSet(static_cast<char*>(d->shm->data()) + ba.size(), 0xff, d->shm->size() - ba.size());
#endif
}

void Launcher::semaphoreReleased()
{
  d->safetyTimer.stop();

  QUrl serverAddress;
#ifdef HAVE_SHM
  {
    SharedMemoryLocker locker(d->shm);
    QByteArray ba = QByteArray::fromRawData(static_cast<const char*>(d->shm->data()), d->shm->size());
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadOnly);

    while (Message::canReadMessage(&buffer)) {
      const Message msg = Message::readMessage(&buffer);
      switch (msg.type()) {
        case Protocol::ServerAddress:
        {
          msg.payload() >> serverAddress;
          break;
        }
        default:
          continue;
      }
    }
  }
  delete d->shm;
  d->shm = 0;

  if (serverAddress.isEmpty()) {
    qWarning() << "Unable to receive server address.";
    QCoreApplication::exit(1);
    return;
  }
#else
  serverAddress.setScheme("tcp");
  serverAddress.setHost("127.0.0.1");
  serverAddress.setPort(Endpoint::defaultPort());
#endif

  std::cout << "GammaRay server listening on: " << qPrintable(serverAddress.toString()) << std::endl;

  if (d->options.uiMode() != LaunchOptions::OutOfProcessUi) // inject only, so we are done here
    return;

  // safer, since we will always be running locally, and the server might give us an external address
  if (serverAddress.scheme() == QStringLiteral("tcp"))
    serverAddress.setHost(QStringLiteral("127.0.0.1"));

  startClient(serverAddress);
  d->state |= ClientStarted;
  checkDone();
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

void Launcher::checkDone()
{
  if (d->state == Complete || (d->options.uiMode() != LaunchOptions::OutOfProcessUi && d->state == InjectorFinished)) {
    emit finished();
  }
  else if ((d->state & InjectorFailed) != 0) {
    d->client.terminate();
    if (d->exitCode == 0)
      d->exitCode = 1;
    emit finished();
  }
}

#include "launcher.moc"
