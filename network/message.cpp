#include "message.h"

#include <QDebug>

using namespace GammaRay;

Message::Message(Protocol::ObjectAddress objectAddress) :
  m_objectAddress(objectAddress),
  m_messageType(Protocol::Invalid)
{
}

Message::~Message()
{
}

Protocol::ObjectAddress Message::address() const
{
  return m_objectAddress;
}

Protocol::MessageType Message::type() const
{
  return m_messageType;
}


QDataStream& Message::stream() const
{
  if (!m_stream) {
    m_stream.reset(new QDataStream(&m_buffer, QIODevice::ReadWrite));
    *m_stream << m_objectAddress;
    // TODO also write type automatically here
  }
  return *m_stream;
}

bool Message::canReadMessage(QIODevice* device)
{
  if (device->bytesAvailable() < 4)
    return false;
  const QByteArray buffer = device->peek(4);
  qint32 size;
  QDataStream(buffer) >> size;
  return device->bytesAvailable() >= size + 4;
}

QByteArray Message::internalBuffer() const
{
  return m_buffer;
}

void Message::setInternalBuffer(const QByteArray& buffer)
{
  m_buffer = buffer;
  m_stream.reset(new QDataStream(m_buffer));
  *m_stream >> m_objectAddress >> m_messageType;
}


QDataStream& operator<<(QDataStream& stream, const Message& msg)
{
  Q_ASSERT(msg.address() != Protocol::InvalidObjectAddress);
  stream << qint32(msg.internalBuffer().size()) << msg.internalBuffer();
  Q_ASSERT(stream.status() == QDataStream::Ok);
  return stream;
}

QDataStream& operator>>(QDataStream& stream, Message& msg)
{
  qint32 size;
  QByteArray buffer;
  stream >> size >> buffer;
  Q_ASSERT(size == buffer.size());
  msg.setInternalBuffer(buffer);
  Q_ASSERT(stream.status() == QDataStream::Ok);
  return stream;
}

