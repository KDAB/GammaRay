/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#ifndef QPACKETPROTOCOL_H
#define QPACKETPROTOCOL_H

#include <QtCore/qobject.h>
#include <QtCore/qdatastream.h>

QT_BEGIN_NAMESPACE
class QIODevice;
class QBuffer;
QT_END_NAMESPACE

namespace QmlJsDebugClient {

class QPacket;
class QPacketAutoSend;

class QPacketProtocolPrivate;

class QPacketProtocol : public QObject
{
    Q_OBJECT

public:
    explicit QPacketProtocol(QIODevice *dev, QObject *parent = 0);
    virtual ~QPacketProtocol();

    qint32 maximumPacketSize() const;
    qint32 setMaximumPacketSize(qint32);

    QPacketAutoSend send();
    void send(const QPacket &);

    qint64 packetsAvailable() const;
    QPacket read();

    void clear();

    QIODevice *device();

Q_SIGNALS:
    void readyRead();
    void invalidPacket();
    void packetWritten();

private:
    QPacketProtocolPrivate *d;
};


class QPacket : public QDataStream
{
public:
    QPacket();
    QPacket(const QPacket &);
    virtual ~QPacket();

    void clear();
    bool isEmpty() const;

protected:
    friend class QPacketProtocol;
    QPacket(const QByteArray &ba);
    QByteArray b;
    QBuffer *buf;
};

class QPacketAutoSend : public QPacket
{
public:
    virtual ~QPacketAutoSend();

private:
    friend class QPacketProtocol;
    QPacketAutoSend(QPacketProtocol *);
    QPacketProtocol *p;
};

static const unsigned int MAX_PACKET_SIZE = 0x7FFFFFFF;

class QPacketProtocolPrivate : public QObject
{
    Q_OBJECT

public:
    QPacketProtocolPrivate(QPacketProtocol *parent, QIODevice *_dev)
    : QObject(parent), inProgressSize(-1), maxPacketSize(MAX_PACKET_SIZE),
      dev(_dev)
    {
        Q_ASSERT(4 == sizeof(qint32));

        QObject::connect(this, SIGNAL(readyRead()),
                         parent, SIGNAL(readyRead()));
        QObject::connect(this, SIGNAL(packetWritten()),
                         parent, SIGNAL(packetWritten()));
        QObject::connect(this, SIGNAL(invalidPacket()),
                         parent, SIGNAL(invalidPacket()));
        QObject::connect(dev, SIGNAL(readyRead()),
                         this, SLOT(readyToRead()));
        QObject::connect(dev, SIGNAL(aboutToClose()),
                         this, SLOT(aboutToClose()));
        QObject::connect(dev, SIGNAL(bytesWritten(qint64)),
                         this, SLOT(bytesWritten(qint64)));
    }

Q_SIGNALS:
    void readyRead();
    void packetWritten();
    void invalidPacket();

public Q_SLOTS:
    void aboutToClose()
    {
        inProgress.clear();
        sendingPackets.clear();
        inProgressSize = -1;
    }

    void bytesWritten(qint64 bytes)
    {
        Q_ASSERT(!sendingPackets.isEmpty());

        while (bytes) {
            if (sendingPackets.at(0) > bytes) {
                sendingPackets[0] -= bytes;
                bytes = 0;
            } else {
                bytes -= sendingPackets.at(0);
                sendingPackets.removeFirst();
                emit packetWritten();
            }
        }
    }

    void readyToRead()
    {
        while (true) {
            // Need to get trailing data
            if (-1 == inProgressSize) {
                // We need a size header of sizeof(qint32)
                if (sizeof(qint32) > (uint)dev->bytesAvailable())
                    return;

                // Read size header
                int read = dev->read((char *)&inProgressSize, sizeof(qint32));
                Q_ASSERT(read == sizeof(qint32));
                Q_UNUSED(read);

                // Check sizing constraints
                if (inProgressSize > maxPacketSize) {
                    QObject::disconnect(dev, SIGNAL(readyRead()),
                                        this, SLOT(readyToRead()));
                    QObject::disconnect(dev, SIGNAL(aboutToClose()),
                                        this, SLOT(aboutToClose()));
                    QObject::disconnect(dev, SIGNAL(bytesWritten(qint64)),
                                        this, SLOT(bytesWritten(qint64)));
                    dev = 0;
                    emit invalidPacket();
                    return;
                }

                inProgressSize -= sizeof(qint32);
            } else {
                inProgress.append(dev->read(inProgressSize - inProgress.size()));

                if (inProgressSize == inProgress.size()) {
                    // Packet has arrived!
                    packets.append(inProgress);
                    inProgressSize = -1;
                    inProgress.clear();

                    emit readyRead();
                } else
                    return;
            }
        }
    }

public:
    QList<qint64> sendingPackets;
    QList<QByteArray> packets;
    QByteArray inProgress;
    qint32 inProgressSize;
    qint32 maxPacketSize;
    QIODevice * dev;
};

} // QmlJsDebugClient


#endif
