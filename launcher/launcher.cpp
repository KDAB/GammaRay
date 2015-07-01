/*
  launcher.cpp

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

#include <config-gammaray.h>
#include "launcher.h"

#include "clientlauncher.h"
#include "injector/abstractinjector.h"
#include "injector/injectorfactory.h"
#include "launchoptions.h"
#include "probefinder.h"

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

class SemaphoreWaiter : public QThread
{
  Q_OBJECT
public:
  explicit SemaphoreWaiter(qint64 id, QObject *parent = 0) : QThread(parent), m_id(id) {}
  ~SemaphoreWaiter() {}
  void run()
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

enum State {
  Initial = 0,
  InjectorFinished = 1,
  InjectorFailed = 2,
  ClientStarted = 4,
  Complete = InjectorFinished | ClientStarted
};

struct LauncherPrivate
{
  LauncherPrivate(const LaunchOptions& options)
    : m_options(options),
    #ifdef HAVE_SHM
      m_shm(0),
    #endif
      m_state(Initial)
  {}

  AbstractInjector::Ptr createInjector() const
  {
    if (m_options.injectorType().isEmpty()) {
      if (m_options.isAttach()) {
        return InjectorFactory::defaultInjectorForAttach();
      } else {
        return InjectorFactory::defaultInjectorForLaunch(m_options.probeABI());
      }
    }
    return InjectorFactory::createInjector(m_options.injectorType());
  }

  LaunchOptions m_options;
#ifndef QT_NO_SHAREDMEMORY
  QSharedMemory *m_shm;
#endif
  ClientLauncher m_client;
  QTimer m_safetyTimer;
  int m_state;
  AbstractInjector::Ptr m_injector;
  QProcessEnvironment m_env;
};


Launcher::Launcher(const LaunchOptions& options, QObject* parent)
  : QObject(parent)
  , p(new LauncherPrivate(options))
{
  Q_ASSERT(options.isValid());

  p->m_safetyTimer.setSingleShot(true);
  p->m_safetyTimer.setInterval(60 * 1000);
  connect(&p->m_safetyTimer, SIGNAL(timeout()), SLOT(timeout()));
}

Launcher::~Launcher()
{
  stop();
  p->m_client.waitForFinished();
  delete p;
}

qint64 Launcher::instanceIdentifier() const
{
  if (p->m_options.isAttach())
    return p->m_options.pid();
  return QCoreApplication::applicationPid();
}

void Launcher::stop()
{
    p->m_injector->stop();
}

bool Launcher::start()
{
  const QString probeDll = !p->m_options.probePath().isEmpty() ? p->m_options.probePath() : ProbeFinder::findProbe(QLatin1String(GAMMARAY_PROBE_NAME), p->m_options.probeABI());
  p->m_options.setProbeSetting("ProbePath", QFileInfo(probeDll).absolutePath());

  sendLauncherId();
  sendProbeSettings();
  sendProbeSettingsFallback();

  if (p->m_options.uiMode() != LaunchOptions::InProcessUi) {
    SemaphoreWaiter *semWaiter = new SemaphoreWaiter(instanceIdentifier(), this);
    connect(semWaiter, SIGNAL(semaphoreReleased()), this, SLOT(semaphoreReleased()), Qt::QueuedConnection);
    semWaiter->start();

    p->m_safetyTimer.start();
  }
  p->m_injector = p->createInjector();
  if (!p->m_injector) {
    if (p->m_options.injectorType().isEmpty()) {
      if (p->m_options.isAttach()) {
        injectorError(-1, tr("Uh-oh, there is no default attach injector on this platform."));
      } else {
        injectorError(-1, tr("Uh-oh, there is no default launch injector on this platform."));
      }
    } else {
      injectorError(-1, tr("Injector %1 not found.").arg(p->m_options.injectorType()));
    }
    return false;
  }

  connect(p->m_injector.data(), SIGNAL(started()), this, SIGNAL(started()));
  connect(p->m_injector.data(), SIGNAL(finished()), this, SLOT(injectorFinished()), Qt::QueuedConnection);

  bool success = false;
  if (p->m_options.isLaunch()) {
    success = p->m_injector->launch(p->m_options.launchArguments(), p->m_env, probeDll, QLatin1String("gammaray_probe_inject"));
  }
  if (p->m_options.isAttach()) {
    success = p->m_injector->attach(p->m_options.pid(), probeDll, QLatin1String("gammaray_probe_inject"));
  }

  if (!success) {
    QString errorMessage;
    if (p->m_options.isLaunch())
      errorMessage = tr("Failed to launch target '%1'.").arg(p->m_options.launchArguments().join(" "));
    if (p->m_options.isAttach())
      errorMessage = tr("Failed to attach to target with PID %1.").arg(p->m_options.pid());
    if (!p->m_injector->errorString().isEmpty())
      errorMessage += tr("\nError: %1").arg(p->m_injector->errorString());
    injectorError(p->m_injector->exitCode() ? p->m_injector->exitCode() : 1, errorMessage);
    return false;
  }
  return true;
}

void Launcher::sendLauncherId()
{
  // if we are launching a new process, make sure it knows how to talk to us
  if (p->m_options.isLaunch()) {
    qputenv("GAMMARAY_LAUNCHER_ID", QByteArray::number(instanceIdentifier()));
  } else {
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
    qputenv("GAMMARAY_LAUNCHER_ID", "");
#else
    qunsetenv("GAMMARAY_LAUNCHER_ID");
#endif
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
    msg.payload() << p->m_options.probeSettings();
    msg.write(&buffer);
  }

  buffer.close();

  p->m_shm = new QSharedMemory(QLatin1String("gammaray-") + QString::number(instanceIdentifier()), this);
  if (!p->m_shm->create(qMax(ba.size(), 1024))) { // make sure we have enough space for the answer
    qWarning() << Q_FUNC_INFO << "Failed to obtain shared memory for probe settings:" << p->m_shm->errorString()
      << "- error code (QSharedMemory::SharedMemoryError):" << p->m_shm->error();
    delete p->m_shm;
    p->m_shm = 0;
    return;
  }

  SharedMemoryLocker locker(p->m_shm);
  qMemCopy(p->m_shm->data(), ba.constData(), ba.size());
  if (p->m_shm->size() > ba.size()) // Windows...
    qMemSet(static_cast<char*>(p->m_shm->data()) + ba.size(), 0xff, p->m_shm->size() - ba.size());
#endif
}

void Launcher::sendProbeSettingsFallback()
{
  if (!p->m_options.isAttach())
    return;

  const QHash<QByteArray, QByteArray> probeSettings = p->m_options.probeSettings();
  for (QHash<QByteArray, QByteArray>::const_iterator it = probeSettings.constBegin(); it != probeSettings.constEnd(); ++it)
    qputenv("GAMMARAY_" + it.key(), it.value());
}

void Launcher::semaphoreReleased()
{
  p->m_safetyTimer.stop();

  QUrl serverAddress;
#ifdef HAVE_SHM
  {
    SharedMemoryLocker locker(p->m_shm);
    QByteArray ba = QByteArray::fromRawData(static_cast<const char*>(p->m_shm->data()), p->m_shm->size());
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
  delete p->m_shm;
  p->m_shm = 0;

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

  if (p->m_options.uiMode() != LaunchOptions::OutOfProcessUi) // inject only, so we are done here
    return;

  // safer, since we will always be running locally, and the server might give us an external address
  if (serverAddress.scheme() == "tcp")
    serverAddress.setHost("127.0.0.1");

  startClient(serverAddress);
  p->m_state |= ClientStarted;
  checkDone();
}

void Launcher::startClient(const QUrl& serverAddress)
{
  if (!p->m_client.launch(serverAddress)) {
    qCritical("Unable to launch gammaray-client!");
    QCoreApplication::exit(1);
  }
}

void Launcher::injectorFinished()
{
  if ((p->m_state & InjectorFailed) == 0)
    p->m_state |= InjectorFinished;
  checkDone();
}

void Launcher::injectorError(int exitCode, const QString& errorMessage)
{
  p->m_state |= InjectorFailed;
  std::cerr << qPrintable(errorMessage) << std::endl;
  std::cerr << "See <https://github.com/KDAB/GammaRay/wiki/Known-Issues> for troubleshooting" <<  std::endl;
  QCoreApplication::exit(exitCode);
}

void Launcher::timeout()
{
  std::cerr << "Target not responding - timeout." << std::endl;
  std::cerr << "See <https://github.com/KDAB/GammaRay/wiki/Known-Issues> for troubleshooting" <<  std::endl;
  p->m_client.terminate();
  QCoreApplication::exit(1);
}

void Launcher::checkDone()
{
  if (p->m_state == Complete || (p->m_options.uiMode() != LaunchOptions::OutOfProcessUi && p->m_state == InjectorFinished)) {
    emit finished();
  }
}

} // namespace GammaRay

#include "launcher.moc"
