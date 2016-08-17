/*
  server.h

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

#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include "gammaray_core_export.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QUdpSocket;
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class MultiSignalMapper;
class ServerDevice;

/** Server side connection endpoint. */
class GAMMARAY_CORE_EXPORT Server : public Endpoint
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    /** Indicates which parts of a QObject should be exported to the client. */
    enum ObjectExportOption {
        ExportNothing = 0x0,
        ExportSignals = 0x1,
        ExportProperties = 0x2,
        ExportEverything = ExportProperties | ExportSignals
    };
    Q_DECLARE_FLAGS(ObjectExportOptions, ObjectExportOption)

    bool listen();
    bool isListening() const;

    /**
     * Register a server-side QObject to send/receive messages to/from the client side.
     * This is equivalent to registerObject(name, object, ExportEverything);
     */
    Protocol::ObjectAddress registerObject(const QString &name, QObject *object) Q_DECL_OVERRIDE;

    /**
     * Register a server-side QObject for exporting to the client.
     */
    Protocol::ObjectAddress registerObject(const QString &name, QObject *object,
                                           ObjectExportOptions exportOptions);

    /**
     * Register a callback slot @p monitorNotifier on object @p receiver that is called if the usage
     * of an object with address @p address changes on the client side.
     *
     * This is useful for example to disable expensive operations like sending large amounts of
     * data if nobody is interested anyway.
     */
    void registerMonitorNotifier(Protocol::ObjectAddress address, QObject *receiver,
                                 const char *monitorNotifier);

    /** Singleton accessor. */
    static Server *instance();

    /**
     * Call @p method on the remote client and also directly on the local object identified by @p objectName.
     */
    void invokeObject(const QString &objectName, const char *method,
                      const QVariantList &args = QVariantList()) const Q_DECL_OVERRIDE;

    bool isRemoteClient() const Q_DECL_OVERRIDE;
    QUrl serverAddress() const Q_DECL_OVERRIDE;
    /**
     * Returns an address suitable to connect to this server.
     * In contrast to serverAddress(), which returns the listening address, which might not
     * be identical for all protocols (such as TCP).
     */
    QUrl externalAddress() const;
protected:
    void messageReceived(const Message &msg) Q_DECL_OVERRIDE;
    void handlerDestroyed(Protocol::ObjectAddress objectAddress,
                          const QString &objectName) Q_DECL_OVERRIDE;
    void objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName,
                         QObject *object) Q_DECL_OVERRIDE;

private slots:
    void newConnection();
    void broadcast();

    /**
     * Forward the signal that triggered the call to this slot to the remote client if connected.
     */
    void forwardSignal(QObject *sender, int signalIndex, const QVector<QVariant> &args);

private:
    void sendServerGreeting();

private:
    ServerDevice *m_serverDevice;
    QHash<Protocol::ObjectAddress, QPair<QObject *, QByteArray> > m_monitorNotifiers;
    Protocol::ObjectAddress m_nextAddress;

    QString m_label;
    QTimer *m_broadcastTimer;

    MultiSignalMapper *m_signalMapper;
};
}

#endif
