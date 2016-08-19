/*
  endpoint.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include "gammaray_common_export.h"
#include "protocol.h"

#include <QMetaMethod>
#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QIODevice;
class QUrl;
QT_END_NAMESPACE

namespace GammaRay {
class Message;
class PropertySyncer;

/** @brief Network protocol endpoint.
 *
 *  Contains:
 *  - object address <-> object name mapping
 *  - message handler registration and message dispatching
 */
class GAMMARAY_COMMON_EXPORT Endpoint : public QObject
{
    Q_OBJECT
public:
    ~Endpoint();

    /** Send @p msg to the connected endpoint. */
    static void send(const Message &msg);

    /** Returns @c true if we are currently connected to another endpoint. */
    static bool isConnected();

    static quint16 defaultPort();
    static quint16 broadcastPort();

    /** Returns the object address for @p objectName, or @c Protocol::InvalidObjectAddress if not known. */
    Protocol::ObjectAddress objectAddress(const QString &objectName) const;

    /** Singleton accessor. */
    static Endpoint *instance();

    /**
     * Register an object of the given name for transparent server/client communication.
     */
    virtual Protocol::ObjectAddress registerObject(const QString &name, QObject *object);

    /**
     * Invoke @p method on the object called @p objectName with the given @p args.
     *
     * This also works with signals.
     *
     * The default implementation forwards the object calls to remote side when the
     * endpoint is connected. The Server implementation is furthermore expected to
     * call the method directly on the local object to support the in-process mode.
     */
    virtual void invokeObject(const QString &objectName, const char *method,
                              const QVariantList &args = QVariantList()) const;

    /**
     * Write all pending data and block until this is done.
     *
     * This should only be used in very rare situations.
     */
    void waitForMessagesWritten();

    /**
     * Returns a human-readable string describing the host program.
     */
    QString label() const;

    /**
     * Sets the human-readable label of this instance used e.g. when advertising on the network.
     */
    void setLabel(const QString &label);

    /**
     * Returns a fixed string uniquely describing the host program.
     */
    QString key() const;

    /**
     * Sets the fixed key of this instance used e.g. when saving settings.
     */
    void setKey(const QString &key);

    /**
     * Returns true for remote clients and false for the in-probe server endpoint.
     */
    virtual bool isRemoteClient() const = 0;

    /**
     * Returns the listening address of the server, in case you need to connect to a different service there
     * (such as the web inspector server).
     */
    virtual QUrl serverAddress() const = 0;

    /** Register the slot @p messageHandlerName on @p receiver as the handler for messages to/from @p objectAddress.
     *  @see dispatchMessage()
     */
    virtual void registerMessageHandler(Protocol::ObjectAddress objectAddress, QObject *receiver,
                                        const char *messageHandlerName);

    /** Unregister the message handler for @p objectAddress. */
    virtual void unregisterMessageHandler(Protocol::ObjectAddress objectAddress);

public slots:
    /** Convenience overload of send(), to directly send message delivered via signals. */
    void sendMessage(const GammaRay::Message &msg);

signals:
    /** Emitted when we lost the connection to the other endpoint. */
    void disconnected();

    /** Emitted when a new object with name @p objectName has been registered at address @p objectAddress. */
    void objectRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void objectUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

protected:
    Endpoint(QObject *parent = 0);
    /** Call with the socket once you have established a connection to another endpoint, takes ownership of @p device. */
    void setDevice(QIODevice *device);

    /** The object address of the other endpoint. */
    Protocol::ObjectAddress endpointAddress() const;

    /** Called for every incoming message.
     *  @see dispatchMessage().
     */
    virtual void messageReceived(const Message &msg) = 0;

    /** Call this when learning about a new object <-> address mapping. */
    void addObjectNameAddressMapping(const QString &objectName,
                                     Protocol::ObjectAddress objectAddress);
    /** Call this when learning about a dissolved object <-> address mapping. */
    void removeObjectNameAddressMapping(const QString &objectName);

    /** Called when the current handler of the object identified by @p objectAddress has been destroyed. */
    virtual void handlerDestroyed(Protocol::ObjectAddress objectAddress,
                                  const QString &objectName) = 0;

    /** Called when a registered object identified by @p objectAddress has been destroyed. */
    virtual void objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName,
                                 QObject *object) = 0;

    /** Calls the message handler registered for the receiver of @p msg. */
    void dispatchMessage(const GammaRay::Message &msg);

    /** Sends a given message. */
    virtual void doSendMessage(const Message &msg);

    /** All current object name/address pairs. */
    QVector<QPair<Protocol::ObjectAddress, QString> > objectAddresses() const;

    /** Singleton instance. */
    static Endpoint *s_instance;

    /**
     * Invoke @p method on @p object with the given @p args.
     *
     * This is invokes the method directly on the local object.
     */
    void invokeObjectLocal(QObject *object, const char *method, const QVariantList &args) const;

    PropertySyncer *m_propertySyncer;

private slots:
    void readyRead();
    void connectionClosed();
    void handlerDestroyed(QObject *obj);
    void objectDestroyed(QObject *obj);

private:
    struct ObjectInfo
    {
        ObjectInfo()
            : address(Protocol::InvalidObjectAddress)
            , object(0)
            , receiver(0)
        {
        }

        QString name;
        Protocol::ObjectAddress address;
        // the locally registered object
        QObject *object;

        // custom message handling support
        QObject *receiver;
        QMetaMethod messageHandler;
    };

    /** Inserts @p oi into all maps. */
    void insertObjectInfo(ObjectInfo *oi);
    /** Removes @p oi from all maps and destroys it. */
    void removeObjectInfo(ObjectInfo *oi);

    QHash<QString, ObjectInfo *> m_nameMap;
    QHash<Protocol::ObjectAddress, ObjectInfo *> m_addressMap;
    QHash<QObject *, ObjectInfo *> m_objectMap;
    QMultiHash<QObject *, ObjectInfo *> m_handlerMap;

    QPointer<QIODevice> m_socket;
    Protocol::ObjectAddress m_myAddress;

    QString m_label;
    QString m_key;
};
}

#endif // GAMMARAY_ENDPOINT_H
