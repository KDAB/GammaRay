#include "message.h"

#include <QDebug>

using namespace GammaRay;

Message::Message() :
  m_objectAddress(Protocol::InvalidObjectAddress),
  m_messageType(Protocol::InvalidMessageType)
{
}

Message::Message(Protocol::ObjectAddress objectAddress, Protocol::MessageType type) :
  m_objectAddress(objectAddress),
  m_messageType(type)
{
}

#ifdef Q_COMPILER_RVALUE_REFS
Message::Message(Message&& other) :
  m_buffer(std::move(other.m_buffer)),
  m_objectAddress(other.m_objectAddress),
  m_messageType(other.m_messageType)
{
  m_stream.swap(other.m_stream);
}
#endif

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
    *m_stream << m_objectAddress << m_messageType;
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

Message Message::readMessage(QIODevice* device)
{
  Message msg;
  QDataStream stream(device);

  qint32 size;
  stream >> size >> msg.m_buffer;
  Q_ASSERT(size == msg.m_buffer.size());
  Q_ASSERT(stream.status() == QDataStream::Ok);

  msg.m_stream.reset(new QDataStream(msg.m_buffer));
  *msg.m_stream >> msg.m_objectAddress >> msg.m_messageType;
  Q_ASSERT(msg.m_stream->status() == QDataStream::Ok);
  Q_ASSERT(msg.m_messageType != Protocol::InvalidMessageType);

  return msg;
}

QByteArray Message::internalBuffer() const
{
  return m_buffer;
}

QDataStream& operator<<(QDataStream& stream, const Message& msg)
{
  Q_ASSERT(msg.address() != Protocol::InvalidObjectAddress);
  stream << qint32(msg.internalBuffer().size()) << msg.internalBuffer();
  Q_ASSERT(stream.status() == QDataStream::Ok);
  return stream;
}
