/*
  probesettings.cpp

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

#include "config-gammaray.h"
#include "probesettings.h"

#include <common/sharedmemorylocker.h>
#include <common/message.h>
#include <common/paths.h>

#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSharedMemory>
#include <QSystemSemaphore>

using namespace GammaRay;

static QHash<QByteArray, QByteArray> s_probeSettings;

QVariant ProbeSettings::value(const QString& key, const QVariant& defaultValue)
{
  QByteArray v = s_probeSettings.value(key.toUtf8());
  if (v.isEmpty())
    v = qgetenv("GAMMARAY_" + key.toLocal8Bit());
  if (v.isEmpty())
    return defaultValue;

  switch (defaultValue.type()) {
    case QVariant::String:
      return QString::fromUtf8(v);
    case QVariant::Bool:
      return v == "true" || v == "1" || v == "TRUE";
    case QVariant::Int:
      return v.toInt();
    default:
      return v;
  }
}

void ProbeSettings::receiveSettings()
{
#ifdef HAVE_SHM
  QSharedMemory shm(QLatin1String("gammaray-") + QString::number(launcherIdentifier()));
  if (!shm.attach()) {
#if QT_VERSION < 0x040800
    qWarning() << "Unable to receive probe settings, cannot attach to shared memory region" << shm.key() << ", error is:" << shm.errorString();
#else
    qWarning() << "Unable to receive probe settings, cannot attach to shared memory region" << shm.key() << shm.nativeKey() << ", error is:" << shm.errorString();
#endif
    qWarning() << "Continueing anyway, with default settings.";

    // see if we got fallback data via environment variables
    const QString probePath = value("ProbePath").toString();
    if (!probePath.isEmpty())
      Paths::setRootPath(probePath + QDir::separator() + GAMMARAY_INVERSE_PROBE_DIR);

    return;
  }
  SharedMemoryLocker locker(&shm);

  QByteArray ba = QByteArray::fromRawData(static_cast<const char*>(shm.data()), shm.size());
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);

  while (Message::canReadMessage(&buffer)) {
    const Message msg = Message::readMessage(&buffer);
    switch (msg.type()) {
      case Protocol::ServerVersion:
      {
        qint32 version;
        msg.payload() >> version;
        if (version != Protocol::version()) {
          qWarning() << "Unable to receive probe settings, mismatching protocol versions (expected:" << Protocol::version() << "got:" << version << ")";
          qWarning() << "Continueing anyway, but this is likely going to fail.";
          return;
        }
        break;
      }
      case Protocol::ProbeSettings:
      {
        msg.payload() >> s_probeSettings;
        //qDebug() << Q_FUNC_INFO << s_probeSettings;
        const QString probePath = value("ProbePath").toString();
        if (!probePath.isEmpty())
          Paths::setRootPath(probePath + QDir::separator() + GAMMARAY_INVERSE_PROBE_DIR);
      }
      default:
        continue;
    }
  }
#endif
}

qint64 ProbeSettings::launcherIdentifier()
{
  bool ok;
  const qint64 id = qgetenv("GAMMARAY_LAUNCHER_ID").toLongLong(&ok);
  if (ok && id > 0)
    return id;
  return QCoreApplication::applicationPid();
}

void ProbeSettings::sendPort(quint16 port)
{
#ifdef HAVE_SHM
  QSharedMemory shm(QLatin1String("gammaray-") + QString::number(launcherIdentifier()));
  if (!shm.attach()) {
#if QT_VERSION < 0x040800
    qWarning() << "Unable to receive probe settings, cannot attach to shared memory region" << shm.key() << ", error is:" << shm.errorString();
#else
    qWarning() << "Unable to receive probe settings, cannot attach to shared memory region" << shm.key() << shm.nativeKey() << ", error is:" << shm.errorString();
#endif
    qWarning() << "Continueing anyway, with default settings.";
    return;
  }

  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  {
    Message msg(Protocol::LauncherAddress, Protocol::ServerPort);
    msg.payload() << port;
    msg.write(&buffer);
  }
  buffer.close();

  if(shm.size() < ba.size())
    qFatal("SHM region too small!");

  {
    SharedMemoryLocker locker(&shm);
    qMemCopy(shm.data(), ba.constData(), ba.size());
    qMemSet(static_cast<char*>(shm.data()) + ba.size(), 0xff, shm.size() - ba.size());
  }

  QSystemSemaphore sem("gammaray-semaphore-" + QString::number(launcherIdentifier()), QSystemSemaphore::Open);
  sem.release();
#else
  Q_UNUSED(port);
#endif
}
