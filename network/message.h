#ifndef GAMMARAY_MESSAGE_H
#define GAMMARAY_MESSAGE_H

#include <QByteArray>
#include <QDataStream>

namespace GammaRay {

/**
 * Single message send between client and server.
 * Binary format: 4 byte size in QDataStream encoding, followed by the message payload
 */
class Message
{
  public:
    Message();
    ~Message();

    QDataStream& stream() const;

    // TODO: sender/receiver

    static bool canReadMessage(QIODevice *device);

    QByteArray internalBuffer() const;
    void setInternalBuffer(const QByteArray &buffer);

  private:
    mutable QByteArray m_buffer;
    mutable QScopedPointer<QDataStream> m_stream;
};

}

QDataStream& operator<<(QDataStream& stream, const GammaRay::Message &msg);
QDataStream& operator>>(QDataStream& stream, GammaRay::Message &msg);

#endif