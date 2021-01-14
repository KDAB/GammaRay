/*
  message.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MESSAGE_H
#define GAMMARAY_MESSAGE_H

#include "gammaray_common_export.h"
#include "protocol.h"

#include <QByteArray>
#include <QDataStream>

#include <functional>
#include <memory>

class MessageBuffer;

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
     * Construct a new message to/from @p objectAddress and message type @p type.
     */
    explicit Message(Protocol::ObjectAddress objectAddress, Protocol::MessageType type);
    Message(Message &&other) Q_DECL_NOEXCEPT; // krazy:exclude=explicit
    ~Message();

    Protocol::ObjectAddress address() const;
    Protocol::MessageType type() const;

    /** Read value from the payload
     *  This operator proxy over payload() allow to do:
     *   - Run time check on the stream status
     */
    template <typename T>
    GammaRay::Message &operator>>(T &value)
    {
        if (Q_UNLIKELY(payload().status() != QDataStream::Ok)) {
            qWarning("%s: Attempting to read from a non valid stream: status: %i", Q_FUNC_INFO, int(payload().status()));
        }
        payload() >> value;
        if (Q_UNLIKELY(payload().status() != QDataStream::Ok)) {
            qWarning("%s: Read from a non valid stream: status: %i", Q_FUNC_INFO, int(payload().status()));
        }
        return *this;
    }

    /** Read value from the payload
     *  This overload allow to read content from a const Message.
     */
    template <typename T>
    GammaRay::Message &operator>>(T &value) const
    {
        return const_cast<GammaRay::Message *>(this)->operator>>(value);
    }

    /** Write value to the payload.
     *  This operator proxy over payload() allow to do:
     *   - Run time check on the stream status
     */
    template <typename T>
    GammaRay::Message &operator<<(const T &value)
    {
        if (Q_UNLIKELY(payload().status() != QDataStream::Ok)) {
            qWarning("%s: Attempting to write to a non valid stream: status: %i", Q_FUNC_INFO, int(payload().status()));
        }
        payload() << value;
        if (Q_UNLIKELY(payload().status() != QDataStream::Ok)) {
            qWarning("%s: Write to a non valid stream: status: %i", Q_FUNC_INFO, int(payload().status()));
        }
        return *this;
    }

    /** Checks if there is a full message waiting in @p device. */
    static bool canReadMessage(QIODevice *device);
    /** Read the next message from @p device. */
    static Message readMessage(QIODevice *device);

    static quint8 lowestSupportedDataVersion();
    static quint8 highestSupportedDataVersion();

    static quint8 negotiatedDataVersion();
    static void setNegotiatedDataVersion(quint8 version);
    static void resetNegotiatedDataVersion();

    /** Write this message to @p device. */
    void write(QIODevice *device) const;

    /** Size of the uncompressed message payload. */
    int size() const;

private:
    Message();

    /** Access to the message payload. This is read-only for received messages
     *  and write-only for messages to be sent.
     */
    QDataStream &payload() const;

    Protocol::ObjectAddress m_objectAddress;
    Protocol::MessageType m_messageType;

    std::unique_ptr<MessageBuffer, std::function<void(MessageBuffer *)>> m_buffer;
};
}

#endif
