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


QDataStream& Message::payload() const
{
  if (!m_stream) {
    m_stream.reset(new QDataStream(&m_buffer, QIODevice::WriteOnly));
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
  Q_ASSERT(size > 0);
  return device->bytesAvailable() >= size + 4;
}

Message Message::readMessage(QIODevice* device)
{
  Message msg;
  QDataStream stream(device);

  qint32 size;
  stream >> size >> msg.m_buffer;
  Q_ASSERT(size >= 2); // at least address and type
  Q_ASSERT(size == msg.m_buffer.size());
  Q_ASSERT(stream.status() == QDataStream::Ok);

  msg.m_stream.reset(new QDataStream(msg.m_buffer));
  *msg.m_stream >> msg.m_objectAddress >> msg.m_messageType;
  Q_ASSERT(msg.m_stream->status() == QDataStream::Ok);
  Q_ASSERT(msg.m_messageType != Protocol::InvalidMessageType);

  return msg;
}

void Message::write(QIODevice* device) const
{
  payload(); // HACK for messages without payload, to ensure address and type are in m_buffer
  Q_ASSERT(m_objectAddress != Protocol::InvalidObjectAddress);
  Q_ASSERT(m_messageType != Protocol::InvalidMessageType);
  Q_ASSERT(m_buffer.size() >= 2); // at least address and type

  // TODO do this manually, we write the size twice this way!
  QDataStream stream(device);
  stream << qint32(m_buffer.size()) << m_buffer;
  Q_ASSERT(stream.status() == QDataStream::Ok);
}
