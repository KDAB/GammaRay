/*
  probesettings.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>
#include "probesettings.h"

#include "common/message.h"
#include "common/paths.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include <QMutex>
#include <QUrl>
#include <QThread>
#include <QWaitCondition>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

#include <iostream>

using namespace GammaRay;

namespace GammaRay {
class ProbeSettingsReceiver;

struct ProbeSettingsData
{
    QHash<QByteArray, QByteArray> settings;
    ProbeSettingsReceiver *receiver;
};

Q_GLOBAL_STATIC(ProbeSettingsData, s_probeSettings)

class ProbeSettingsReceiver : public QObject
{
    Q_OBJECT
public:
    explicit ProbeSettingsReceiver(QObject *parent = nullptr);
    ~ProbeSettingsReceiver() override;
    Q_INVOKABLE void sendServerAddress(const QUrl &address);
    Q_INVOKABLE void sendServerLaunchError(const QString &reason);

    void waitForSettingsReceived();

private slots:
    void readyRead();
    void settingsReceivedFallback();

private:
    Q_INVOKABLE void run();
    static void setRootPathFromProbePath(const QString &probePath);
    QLocalSocket *m_socket = nullptr;
    QWaitCondition m_waitCondition;
    QMutex m_mutex;
};

ProbeSettingsReceiver::ProbeSettingsReceiver(QObject *parent)
    : QObject(parent)
{
}

ProbeSettingsReceiver::~ProbeSettingsReceiver()
{
    delete m_socket;
}

void ProbeSettingsReceiver::run()
{
    m_mutex.lock(); // we only need this for ordering run after waitForSettingsReceived
    m_mutex.unlock();

    m_socket = new QLocalSocket;
    connect(m_socket, &QLocalSocket::disconnected, this, &ProbeSettingsReceiver::settingsReceivedFallback);
    connect(m_socket, &QLocalSocket::errorOccurred, this, &ProbeSettingsReceiver::settingsReceivedFallback);
    connect(m_socket, &QIODevice::readyRead, this, &ProbeSettingsReceiver::readyRead);
    m_socket->connectToServer(QStringLiteral("gammaray-")
                              + QString::number(ProbeSettings::launcherIdentifier()));
    if (!m_socket->waitForConnected(10000)) {
#ifndef Q_OS_ANDROID
        qWarning() << "Failed to connect to launcher, can't receive probe settings!"
                   << m_socket->errorString();
#endif
        settingsReceivedFallback();
    }
}

void ProbeSettingsReceiver::waitForSettingsReceived()
{
    m_mutex.lock();
    QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection);
    m_waitCondition.wait(&m_mutex);
    m_mutex.unlock();
}

void ProbeSettingsReceiver::readyRead()
{
    while (Message::canReadMessage(m_socket)) {
        auto msg = Message::readMessage(m_socket);
        switch (msg.type()) {
        case Protocol::ServerVersion: {
            qint32 version;
            msg >> version;
            if (version != Protocol::version()) {
                qWarning()
                    << "Unable to receive probe settings, mismatching protocol versions (expected:"
                    << Protocol::version() << "got:" << version << ")";
                qWarning() << "Continuing anyway, but this is likely going to fail.";
                settingsReceivedFallback();
                return;
            }
            break;
        }
        case Protocol::ProbeSettings: {
            msg >> s_probeSettings()->settings;
            // qDebug() << Q_FUNC_INFO << s_probeSettings()->settings;
            const QString probePath = ProbeSettings::value(QStringLiteral("ProbePath")).toString();
            setRootPathFromProbePath(probePath);

            m_waitCondition.wakeAll();
            return;
        }
        default:
            continue;
        }
    }
}

static void waitForBytesWritten(QLocalSocket &socket)
{
    while (socket.state() == QLocalSocket::ConnectedState && socket.bytesToWrite() > 0) {
        if (!socket.waitForBytesWritten()) {
            qWarning() << Q_FUNC_INFO << "Failed to wait for bytes written";
            break;
        }
    }
}

void ProbeSettingsReceiver::sendServerAddress(const QUrl &address)
{
    if (!m_socket || m_socket->state() != QLocalSocket::ConnectedState)
        return;

    Message msg(Protocol::LauncherAddress, Protocol::ServerAddress);
    msg << address;
    msg.write(m_socket);

    waitForBytesWritten(*m_socket);
    m_socket->close();

    deleteLater();
    s_probeSettings()->receiver = nullptr;
    thread()->quit();
}

void ProbeSettingsReceiver::sendServerLaunchError(const QString &reason)
{
    if (!m_socket || m_socket->state() != QLocalSocket::ConnectedState)
        return;

    Message msg(Protocol::LauncherAddress, Protocol::ServerLaunchError);
    msg << reason;
    msg.write(m_socket);

    waitForBytesWritten(*m_socket);
    m_socket->close();

    deleteLater();
    s_probeSettings()->receiver = nullptr;
    thread()->quit();
}

void ProbeSettingsReceiver::settingsReceivedFallback()
{
    // see if we got fallback data via environment variables
    const QString probePath = ProbeSettings::value(QStringLiteral("ProbePath")).toString();
    setRootPathFromProbePath(probePath);

    m_waitCondition.wakeAll();
}

void ProbeSettingsReceiver::setRootPathFromProbePath(const QString &probePath)
{
    if (probePath.isEmpty())
        return;

    QFileInfo fi(probePath);
    if (fi.isFile())
        Paths::setRootPath(fi.absolutePath() + QDir::separator() + GAMMARAY_INVERSE_PROBE_DIR);
    else
        Paths::setRootPath(probePath + QDir::separator() + GAMMARAY_INVERSE_PROBE_DIR);
}
}

#ifdef QT_ANDROIDEXTRAS_LIB
static QVariant getPackageMetaData(const QString &key, const QVariant &defaultValue)
{
    auto pm = QtAndroid::androidContext().callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
    auto packageName = QtAndroid::androidContext().callObjectMethod("getPackageName", "()Ljava/lang/String;");
    auto GET_META_DATA = QAndroidJniObject::getStaticField<jint>("android/content/pm/PackageManager", "GET_META_DATA");
    auto appInfo = pm.callObjectMethod("getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;", packageName.object(), GET_META_DATA);
    auto metaData = appInfo.getObjectField("metaData", "Landroid/os/Bundle;");
    if (!metaData.isValid()) {
        return defaultValue;
    }

    // TODO handle different type cases based on defaultValue.type()
    auto value = metaData.callObjectMethod("getString", "(Ljava/lang/String;)Ljava/lang/String;", QAndroidJniObject::fromString(key).object());
    if (value.isValid()) {
        return value.toString();
    }

    return defaultValue;
}
#endif

QVariant ProbeSettings::value(const QString &key, const QVariant &defaultValue)
{
    QByteArray v = s_probeSettings()->settings.value(key.toUtf8());
    if (v.isEmpty()) {
        const QByteArray cstr = "GAMMARAY_" + key.toLocal8Bit();
        v = qgetenv(cstr);
    }

#ifdef QT_ANDROIDEXTRAS_LIB
    if (v.isEmpty()) {
        return getPackageMetaData("com.kdab.gammaray." + key, defaultValue);
    }
#endif

    if (v.isEmpty())
        return defaultValue;

    switch (defaultValue.typeId()) {
    case QMetaType::QString:
        return QString::fromUtf8(v);
    case QMetaType::Bool:
        return v == "true" || v == "1" || v == "TRUE";
    case QMetaType::Int:
        return v.toInt();
    default:
        return v;
    }
}

void ProbeSettings::receiveSettings()
{
    auto t = new QThread;
    QObject::connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
    auto receiver = new ProbeSettingsReceiver;
    s_probeSettings()->receiver = receiver;
    receiver->moveToThread(t);
    receiver->waitForSettingsReceived();
}

qint64 ProbeSettings::launcherIdentifier()
{
    bool ok;
    const qint64 id = qgetenv("GAMMARAY_LAUNCHER_ID").toLongLong(&ok);
    if (ok && id > 0)
        return id;
    return QCoreApplication::applicationPid();
}

void ProbeSettings::resetLauncherIdentifier()
{
    // if we were launch by GammaRay, and we later try to re-attach, we need to make sure
    // to not end up with an outdated launcher id
    qputenv("GAMMARAY_LAUNCHER_ID", "");
}

void ProbeSettings::sendServerAddress(const QUrl &addr)
{
    Q_ASSERT(s_probeSettings()->receiver);
    QMetaObject::invokeMethod(s_probeSettings()->receiver, "sendServerAddress", Q_ARG(QUrl, addr));
}

void ProbeSettings::sendServerLaunchError(const QString &reason)
{
    Q_ASSERT(s_probeSettings()->receiver);
    QMetaObject::invokeMethod(s_probeSettings()->receiver, "sendServerLaunchError", Q_ARG(QString, reason));
}

#include "probesettings.moc"
