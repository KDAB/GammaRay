#include "message.h"

#include <QDebug>

using namespace GammaRay;

Message::Message()
{
}

Message::~Message()
{
}

QDataStream& Message::stream() const
{
  if (!m_stream)
    m_stream.reset(new QDataStream(&m_buffer, QIODevice::ReadWrite));
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
}


QDataStream& operator<<(QDataStream& stream, const Message& msg)
{
  qDebug() << Q_FUNC_INFO << msg.internalBuffer().size() << stream.device() << stream.status();
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

