/*
  message.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    template<typename T>
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
    template<typename T>
    GammaRay::Message &operator>>(T &value) const
    {
        return const_cast<GammaRay::Message *>(this)->operator>>(value);
    }

    /** Write value to the payload.
     *  This operator proxy over payload() allow to do:
     *   - Run time check on the stream status
     */
    template<typename T>
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

    /** Current position of the stream */
    int pos() const;

    /**
     * Finds @p marker and seeks the internal QDataStream to
     * right after the marker.
     *
     * @p from the pos to start search from
     */
    void findAndSkipCString(const char *marker, int from) const;

    /**
     * Write a c string (starting at @p bytes, with size @p len) to the message.
     * This method can be used to add a marker with the data
     * that you are writing. This marker can then be used
     * to skip a certain portion of the message in case
     * the stream is not valid anymore. Use the method
     * findAndSkipCString to skip this marker when reading
     * the message.
     * @return the number of bytes written
     */
    int writeCStringMarker(const char *bytes, int len);

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
