/*
  clientdevice.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTDEVICE_H
#define GAMMARAY_CLIENTDEVICE_H

#include <QObject>
#include <QUrl>

class QIODevice;

namespace GammaRay {

/** Adapter for the various different client socket classes (TCP, local, etc). */
class ClientDevice : public QObject
{
    Q_OBJECT
public:
    explicit ClientDevice(QObject* parent = 0);
    ~ClientDevice();

    static ClientDevice* create(const QUrl& url, QObject* parent);

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
};

template <typename ClientT>
class ClientDeviceImpl : public ClientDevice
{
public:
    explicit ClientDeviceImpl(QObject *parent = 0) : ClientDevice(parent), m_socket(0)
    {
    }

    inline QIODevice* device() const
    {
        return m_socket;
    }

protected:
    ClientT* m_socket;
};

}

#endif // GAMMARAY_CLIENTDEVICE_H
