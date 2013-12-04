#include "launcher.h"

#include <include/sharedmemorylocker.h>
#include <network/message.h>

#include <QByteArray>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QThread>

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
    qDebug() << Q_FUNC_INFO << "pre aquire";
    sem.acquire();
    qDebug() << Q_FUNC_INFO << "post aquire" << sem.errorString();
    emit semaphoreReleased();
  }

signals:
  void semaphoreReleased();

private:
  qint64 m_id;
};


Launcher::Launcher(const LaunchOptions& options, QObject* parent):
  QObject(parent),
  m_options(options),
  m_shm(0)
{
  Q_ASSERT(options.isValid());

  // wait for the event loop to be available
  QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

Launcher::~Launcher()
{
}

qint64 Launcher::instanceIdentifier() const
{
  if (m_options.isAttach())
    return m_options.pid();
  return QCoreApplication::applicationPid();
}

void Launcher::delayedInit()
{
  sendLauncherId();
  sendProbeSettings();
  sendProbeSettingsFallback();

  // TODO out-of-process only
  SemaphoreWaiter *semWaiter = new SemaphoreWaiter(instanceIdentifier(), this);
  connect(semWaiter, SIGNAL(semaphoreReleased()), this, SLOT(semaphoreReleased()), Qt::DirectConnection); // TODO make queued connection once injector moved to a thread
  semWaiter->start();

  // TODO start async launch/attach

  // TODO wait for port (out-of-process only)

  // TODO launch client (out-of-process only)


  // ### temporary until the code from main moved here
  QCoreApplication::quit();
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
  qDebug() << Q_FUNC_INFO;
  SharedMemoryLocker locker(m_shm);
  quint16 port = *reinterpret_cast<const quint16*>(m_shm->constData());
  qDebug() << "port: " << port;
}

#include "launcher.moc"
