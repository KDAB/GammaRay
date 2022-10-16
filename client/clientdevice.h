/*
  clientdevice.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLIENTDEVICE_H
#define GAMMARAY_CLIENTDEVICE_H

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

namespace GammaRay {
/** Adapter for the various different client socket classes (TCP, local, etc). */
class ClientDevice : public QObject
{
    Q_OBJECT
public:
    explicit ClientDevice(QObject *parent = nullptr);
    ~ClientDevice();

    static ClientDevice *create(const QUrl &url, QObject *parent);

    void setTryAgain(int tries);
    virtual void connectToHost() = 0;
    virtual void disconnectFromHost() = 0;
    virtual QIODevice *device() const = 0;

signals:
    void connected();
    /** Server is not up yet. */
    void transientError();
    /** Anything not being a transient error. */
    void persistentError(const QString &errorMsg);

protected:
    QUrl m_serverAddress;
    int m_tries;
};

template<typename ClientT>
class ClientDeviceImpl : public ClientDevice
{
public:
    explicit ClientDeviceImpl(QObject *parent = nullptr)
        : ClientDevice(parent)
        , m_socket(nullptr)
    {
    }

    QIODevice *device() const override
    {
        return m_socket;
    }

protected:
    ClientT *m_socket;
};
}

#endif // GAMMARAY_CLIENTDEVICE_H
