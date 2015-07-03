/*
  message.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#include "message.h"

#include "lz4/lz4.h" //3rdparty

#include <QDebug>
#include <qendian.h>

inline QByteArray compress(const QByteArray &src)
{
    const qint32 srcSz = src.size();

    QByteArray dst;
    dst.resize(LZ4_compressBound(srcSz + sizeof(srcSz)));
    *(qint32*)dst.data() = srcSz; // save the source size

    const int sz = LZ4_compress_default(src.constData(), dst.data() + sizeof(int), srcSz, dst.size());
    dst.resize(sz + sizeof(srcSz));
    return dst;
}

inline QByteArray uncompress(const QByteArray &src)
{
    const qint32 dstSz = *(const qint32*)src.constData(); // get the dest size
    QByteArray dst;
    dst.resize(dstSz);
    const int sz = LZ4_decompress_safe(src.constData() + sizeof(dstSz), dst.data(), src.size()- sizeof(dstSz), dstSz);
    if (sz <= 0)
        dst.resize(0);
    else
        dst.resize(sz);
    return dst;
}

static const QDataStream::Version StreamVersion = QDataStream::Qt_4_7;
#ifdef ENABLE_MESSAGE_COMPRESSSION
static const int minimumUncompressedSize = 32;
#endif

#if QT_VERSION < 0x040800
// This template-specialization is missing in qendian.h, required for qFromBigEndian
template<> inline quint8 qbswap<quint8>(quint8 source)
{
    return source;
}
#endif

template<typename T> static T readNumber(QIODevice *device)
{
  T buffer;
  const int readSize = device->read((char*)&buffer, sizeof(T));
  Q_UNUSED(readSize);
  Q_ASSERT(readSize == sizeof(T));
  return qFromBigEndian(buffer);
}

template<typename T> static void writeNumber(QIODevice *device, T value)
{
  value = qToBigEndian(value);
  const int writeSize = device->write((char*)&value, sizeof(T));
  Q_UNUSED(writeSize);
  Q_ASSERT(writeSize == sizeof(T));
}

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
    if (m_buffer.isEmpty())
      m_stream.reset(new QDataStream(&m_buffer, QIODevice::WriteOnly));
    else
      m_stream.reset(new QDataStream(m_buffer));
    m_stream->setVersion(StreamVersion);
  }
  return *m_stream;
}

bool Message::canReadMessage(QIODevice* device)
{
  static const int minimumSize = sizeof(Protocol::PayloadSize) + sizeof(Protocol::ObjectAddress) + sizeof(Protocol::MessageType);
  if (device->bytesAvailable() < minimumSize)
    return false;

  Protocol::PayloadSize payloadSize;
  const int peekSize = device->peek((char*)&payloadSize, sizeof(Protocol::PayloadSize));
  if (peekSize < (int)sizeof(Protocol::PayloadSize))
    return false;

  if (payloadSize == -1 && !device->isSequential()) // input end on shared memory
    return false;

  payloadSize = abs(qFromBigEndian(payloadSize));
  return device->bytesAvailable() >= payloadSize + minimumSize;
}

Message Message::readMessage(QIODevice* device)
{
  Message msg;

  Protocol::PayloadSize payloadSize = readNumber<qint32>(device);

  msg.m_objectAddress = readNumber<Protocol::ObjectAddress>(device);
  msg.m_messageType = readNumber<Protocol::MessageType>(device);
  Q_ASSERT(msg.m_messageType != Protocol::InvalidMessageType);
  Q_ASSERT(msg.m_objectAddress != Protocol::InvalidObjectAddress);
  if (payloadSize < 0) {
    payloadSize = abs(payloadSize);
    QByteArray buff = device->read(payloadSize);
    msg.m_buffer = uncompress(buff);
    Q_ASSERT(payloadSize == buff.size());
  } else {
      if (payloadSize > 0) {
          msg.m_buffer = device->read(payloadSize);
          Q_ASSERT(payloadSize == msg.m_buffer.size());
      }
  }
  return msg;
}

void Message::write(QIODevice* device) const
{
  Q_ASSERT(m_objectAddress != Protocol::InvalidObjectAddress);
  Q_ASSERT(m_messageType != Protocol::InvalidMessageType);
  const int buffSize = m_buffer.size();
#ifdef ENABLE_MESSAGE_COMPRESSSION
  QByteArray buff;
  if (buffSize > minimumUncompressedSize)
    buff = compress(m_buffer);

  if (buff.size() && buff.size() <  buffSize)
      writeNumber<Protocol::PayloadSize>(device, -buff.size()); // send compressed Buffer
  else
#endif
      writeNumber<Protocol::PayloadSize>(device, buffSize); // send uncompressed Buffer

  writeNumber(device, m_objectAddress);
  writeNumber(device, m_messageType);

#ifdef ENABLE_MESSAGE_COMPRESSSION
  if (buffSize) {
      if (buff.size() && buff.size() <  buffSize) {
        const int s = device->write(buff);
        Q_ASSERT(s == buff.size());
        Q_UNUSED(s);
      } else {
#endif
        const int s = device->write(m_buffer);
        Q_ASSERT(s == m_buffer.size());
        Q_UNUSED(s);
#ifdef ENABLE_MESSAGE_COMPRESSSION
      }
  }
#endif
}
