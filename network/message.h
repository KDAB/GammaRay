#ifndef GAMMARAY_MESSAGE_H
#define GAMMARAY_MESSAGE_H

#include "protocol.h"

#include <QByteArray>
#include <QDataStream>

namespace GammaRay {

/**
 * Single message send between client and server.
 * Binary format:
 * - 4 byte size of the entire message (not including the size itself) in QDataStream encoding
 * - 1 byte server object address
 * - 1 byte command type
 * - size-2 bytes message payload
 */
class Message
{
  public:
    explicit Message(Protocol::ObjectAddress address = Protocol::InvalidObjectAddress);
    ~Message();

    Protocol::ObjectAddress address() const;
    Protocol::MessageType type() const;

    QDataStream& stream() const;

    static bool canReadMessage(QIODevice *device);

    QByteArray internalBuffer() const;
    void setInternalBuffer(const QByteArray &buffer);

  private:
    mutable QByteArray m_buffer;
    mutable QScopedPointer<QDataStream> m_stream;

    Protocol::ObjectAddress m_objectAddress;
    Protocol::MessageType m_messageType;
};

}

QDataStream& operator<<(QDataStream& stream, const GammaRay::Message &msg);
QDataStream& operator>>(QDataStream& stream, GammaRay::Message &msg);

#endif