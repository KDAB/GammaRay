/*
  message.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MESSAGE_H
#define GAMMARAY_MESSAGE_H

#include "gammaray_common_export.h"
#include "protocol.h"

#include <QByteArray>
#include <QDataStream>

namespace GammaRay {

/**
 * Single message send between client and server.
 * Binary format:
 * - sizeof(Protocol::PayloadSize) byte size of the message payload (not including the size and other fixed fields itself) in netowork byte order (big endian)
 * - sizeof(Protocol::ObjectAddress) server object address (big endian)
 * - sizeof(Protocol::MessageType) command type (big endian)
 * - size bytes message payload (encoding is user defined, QDataStream provided for convenience)
 */
class GAMMARAY_COMMON_EXPORT Message
{
  public:
    /**
     * Construct an empty message for reading.
     *
     * Ideally this would be private and we'd have a static read method returning the message
     * so that you never can construct an invalid message objects, but that requires a move
     * ctor and thus C++11 support (see Git history, but doesn't work for everyone yet, unfortunately).
     */
    Message();
    /**
     * Construct a new message to/from @p address and message type @p type.
     */
    explicit Message(Protocol::ObjectAddress address, Protocol::MessageType type);
    ~Message();

    Protocol::ObjectAddress address() const;
    Protocol::MessageType type() const;

    /** Access to the message payload. This is read-only for received messages
     *  and write-only for messages to be sent.
     */
    QDataStream& payload() const;

    /** Checks if there is a full message waiting in @p device. */
    static bool canReadMessage(QIODevice *device);
    /** Read the next message from @p device. */
    void read(QIODevice *device);

    /** Write this message to @p device. */
    void write(QIODevice *device) const;

  private:
    mutable QByteArray m_buffer;
    mutable QScopedPointer<QDataStream> m_stream;

    Protocol::ObjectAddress m_objectAddress;
    Protocol::MessageType m_messageType;
};

}

#endif
