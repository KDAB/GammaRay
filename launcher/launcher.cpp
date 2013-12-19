/*
  launcher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "launcher.h"
#include "probefinder.h"
#include "injector/abstractinjector.h"
#include "injector/injectorfactory.h"

#include <common/sharedmemorylocker.h>
#include <common/message.h>

#include <QByteArray>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QThread>

#include <iostream>

using namespace GammaRay;

class SemaphoreWaiter : public QThread
{
  Q_OBJECT
public:
  explicit SemaphoreWaiter(qint64 id, QObject *parent = 0) : QThread(parent), m_id(id) {}
  ~SemaphoreWaiter() {}
  void run()
  {
    QSystemSemaphore sem("gammaray-semaphore-" + QString::number(m_id), 0, QSystemSemaphore::Create);
    sem.acquire();
    emit semaphoreReleased();
  }

signals:
  void semaphoreReleased();

private:
  qint64 m_id;
};

class InjectorThread : public QThread
{
  Q_OBJECT
public:
  explicit InjectorThread(const LaunchOptions &options, const QString &probeDll, QObject *parent = 0)
    : QThread(parent), m_options(options), m_probeDll(probeDll)
  {
    Q_ASSERT(options.isValid());
  }
  ~InjectorThread() {}

  AbstractInjector::Ptr createInjector() const
  {
    if (m_options.injectorType().isEmpty()) {
      if (m_options.isAttach()) {
        return InjectorFactory::defaultInjectorForAttach();
      } else {
        return InjectorFactory::defaultInjectorForLaunch();
      }
    }
    return InjectorFactory::createInjector(m_options.injectorType());
  }

  void run()
  {
    const AbstractInjector::Ptr injector = createInjector();

    if (!injector) {
      if (m_options.injectorType().isEmpty()) {
        if (m_options.isAttach()) {
          emit error(-1, tr("Uh-oh, there is no default attach injector on this platform."));
        } else {
          emit error(-1, tr("Uh-oh, there is no default launch injector on this platform."));
        }
      } else {
        emit error(-1, tr("Injector %1 not found.").arg(m_options.injectorType()));
      }
      return;
    }

    bool success = false;
    if (m_options.isLaunch()) {
      success = injector->launch(m_options.launchArguments(), m_probeDll, QLatin1String("gammaray_probe_inject"));
    }
    if (m_options.isAttach()) {
      success = injector->attach(m_options.pid(), m_probeDll, QLatin1String("gammaray_probe_inject"));
    }

    if (!success) {
      QString errorMessage;
      if (m_options.isLaunch())
        errorMessage = tr("Failed to launch target '%1'.").arg(m_options.launchArguments().join(" "));
      if (m_options.isAttach())
        errorMessage = tr("Failed to attach to target with PID %1.").arg(m_options.pid());
      if (!injector->errorString().isEmpty())
        errorMessage += tr("\nError: %1").arg(injector->errorString());
      emit error(injector->exitCode(), errorMessage);
    }
  }

signals:
  void error(int exitCode, const QString &errorMessage);

private:
  LaunchOptions m_options;
  QString m_probeDll;
};

Launcher::Launcher(const LaunchOptions& options, QObject* parent):
  QObject(parent),
  m_options(options),
  m_shm(0),
  m_state(Initial)
{
  Q_ASSERT(options.isValid());

  m_safetyTimer.setSingleShot(true);
  m_safetyTimer.setInterval(60 * 1000);
  connect(&m_safetyTimer, SIGNAL(timeout()), SLOT(timeout()));

  // wait for the event loop to be available
  QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

Launcher::~Launcher()
{
  m_client.waitForFinished();
}

qint64 Launcher::instanceIdentifier() const
{
  if (m_options.isAttach())
    return m_options.pid();
  return QCoreApplication::applicationPid();
}

void Launcher::delayedInit()
{
  const QString probeDll = ProbeFinder::findProbe(QLatin1String("gammaray_probe"), m_options.probeABI());
  m_options.setProbeSetting("ProbePath", QFileInfo(probeDll).absolutePath());

  sendLauncherId();
  sendProbeSettings();
  sendProbeSettingsFallback();

  if (m_options.uiMode() != LaunchOptions::InProcessUi) {
    SemaphoreWaiter *semWaiter = new SemaphoreWaiter(instanceIdentifier(), this);
    connect(semWaiter, SIGNAL(semaphoreReleased()), this, SLOT(semaphoreReleased()), Qt::QueuedConnection);
    semWaiter->start();

    m_safetyTimer.start();
  }

  InjectorThread *injector = new InjectorThread(m_options, probeDll, this);
  connect(injector, SIGNAL(finished()), this, SLOT(injectorFinished()), Qt::QueuedConnection);
  connect(injector, SIGNAL(error(int,QString)), this, SLOT(injectorError(int,QString)), Qt::QueuedConnection);
  injector->start();
}

void Launcher::sendLauncherId()
{
  // if we are launching a new process, make sure it knows how to talk to us
  if (m_options.isLaunch()) {
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
    msg.payload() << m_options.probeSettings();
    msg.write(&buffer);
  }

  buffer.close();

  m_shm = new QSharedMemory(QLatin1String("gammaray-") + QString::number(instanceIdentifier()), this);
  if (!m_shm->create(ba.size())) {
    qWarning() << Q_FUNC_INFO << "Failed to obtain shared memory for probe settings:" << m_shm->errorString();
    delete m_shm;
    m_shm = 0;
    return;
  }

  SharedMemoryLocker locker(m_shm);
  qMemCopy(m_shm->data(), ba.constData(), ba.size());
  if (m_shm->size() > ba.size()) // Windows...
    qMemSet(static_cast<char*>(m_shm->data()) + ba.size(), 0xff, m_shm->size() - ba.size());
}

void Launcher::sendProbeSettingsFallback()
{
  if (!m_options.isAttach())
    return;

  const QHash<QByteArray, QByteArray> probeSettings = m_options.probeSettings();
  for (QHash<QByteArray, QByteArray>::const_iterator it = probeSettings.constBegin(); it != probeSettings.constEnd(); ++it)
    qputenv("GAMMARAY_" + it.key(), it.value());
}

void Launcher::semaphoreReleased()
{
  m_safetyTimer.stop();

  SharedMemoryLocker locker(m_shm);
  QByteArray ba = QByteArray::fromRawData(static_cast<const char*>(m_shm->data()), m_shm->size());
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);

  quint16 port = 0;

  while (Message::canReadMessage(&buffer)) {
    const Message msg = Message::readMessage(&buffer);
    switch (msg.type()) {
      case Protocol::ServerPort:
      {
        msg.payload() >> port;
        break;
      }
      default:
        continue;
    }
  }

  if (port == 0) {
    qWarning() << "Unable to receive port number.";
    QCoreApplication::exit(1);
    return;
  }

  std::cout << "GammaRay server listening on port: " << port << std::endl;

  if (m_options.uiMode() != LaunchOptions::OutOfProcessUi) // inject only, so we are done here
    return;

  if (!m_client.launch("127.0.0.1", port)) {
    qCritical("Unable to launch gammaray-client!");
    QCoreApplication::exit(1);
  }

  m_state |= ClientStarted;
  checkDone();
}

void Launcher::injectorFinished()
{
  m_state |= InjectorFinished;
  checkDone();
}

void Launcher::injectorError(int exitCode, const QString& errorMessage)
{
  std::cerr << qPrintable(errorMessage) << std::endl;
  QCoreApplication::exit(exitCode);
}

void Launcher::timeout()
{
  std::cerr << "Target not responding - timeout." << std::endl;
  m_client.terminate();
  QCoreApplication::exit(1);
}

void Launcher::checkDone()
{
  if (m_state == Complete || (m_options.uiMode() != LaunchOptions::OutOfProcessUi && m_state == InjectorFinished))
    QCoreApplication::quit();
}

#include "launcher.moc"
