/*
  message.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "message.h"

#include "sharedpool.h"
#include "lz4/lz4.h" // 3rdparty

#include <QBuffer>
#include <QDebug>
#include <qendian.h>

inline void compress(const QByteArray &src, QByteArray &dst)
{
    const qint32 srcSz = src.size();

    dst.resize(LZ4_compressBound(srcSz + sizeof(srcSz)));
    *(qint32 *)dst.data() = srcSz; // save the source size

    const int sz
        = LZ4_compress_default(src.constData(), dst.data() + sizeof(int), srcSz, dst.size());
    dst.resize(sz + sizeof(srcSz));
}

inline void uncompress(const QByteArray &src, QByteArray &dst)
{
    const qint32 dstSz = *(const qint32 *)src.constData(); // get the dest size
    dst.resize(dstSz);
    const int sz = LZ4_decompress_safe(src.constData() + sizeof(dstSz), dst.data(),
                                       src.size()- sizeof(dstSz), dstSz);
    if (sz <= 0)
        dst.resize(0);
    else
        dst.resize(sz);
}

static quint8 s_streamVersion = GammaRay::Message::lowestSupportedDataVersion();
static const int minimumUncompressedSize = 32;

template<typename T> static T readNumber(QIODevice *device)
{
    T buffer;
    const int readSize = device->read((char *)&buffer, sizeof(T));
    Q_UNUSED(readSize);
    Q_ASSERT(readSize == sizeof(T));
    return qFromBigEndian(buffer);
}

template<typename T> static void writeNumber(QIODevice *device, T value)
{
    value = qToBigEndian(value);
    const int writeSize = device->write((char *)&value, sizeof(T));
    Q_UNUSED(writeSize);
    Q_ASSERT(writeSize == sizeof(T));
}

using namespace GammaRay;

class MessageBuffer
{
public:
    MessageBuffer()
        : stream(&data)
    {
        data.open(QIODevice::ReadWrite);

        // explicitly reserve memory so a resize() won't shed it
        data.buffer().reserve(32);
        scratchSpace.reserve(32);
    }

    ~MessageBuffer() = default;

    void clear()
    {
        data.buffer().resize(0);
        resetStatus();
    }

    void resetStatus()
    {
        data.seek(0);
        scratchSpace.resize(0);
        stream.resetStatus();
    }

    QBuffer data;
    QByteArray scratchSpace;
    QDataStream stream;
};

Q_GLOBAL_STATIC_WITH_ARGS(SharedPool<MessageBuffer>, s_sharedMessageBufferPool, (5))

Message::Message()
    : m_objectAddress(Protocol::InvalidObjectAddress)
    , m_messageType(Protocol::InvalidMessageType)
    , m_buffer(s_sharedMessageBufferPool()->acquire())
{
    m_buffer->clear();
    m_buffer->stream.setVersion(s_streamVersion);
}

Message::Message(Protocol::ObjectAddress objectAddress, Protocol::MessageType type)
    : m_objectAddress(objectAddress)
    , m_messageType(type)
    , m_buffer(s_sharedMessageBufferPool()->acquire())
{
    m_buffer->clear();
    m_buffer->stream.setVersion(s_streamVersion);
}

Message::Message(Message &&other) Q_DECL_NOEXCEPT
    : m_objectAddress(other.m_objectAddress)
    , m_messageType(other.m_messageType)
    , m_buffer(std::move(other.m_buffer))
{
}

Message::~Message() = default;

Protocol::ObjectAddress Message::address() const
{
    return m_objectAddress;
}

Protocol::MessageType Message::type() const
{
    return m_messageType;
}

QDataStream &Message::payload() const
{
    return m_buffer->stream;
}

bool Message::canReadMessage(QIODevice *device)
{
    if (!device)
        return false;

    static const int minimumSize = sizeof(Protocol::PayloadSize) + sizeof(Protocol::ObjectAddress)
                                   + sizeof(Protocol::MessageType);
    if (device->bytesAvailable() < minimumSize)
        return false;

    Protocol::PayloadSize payloadSize;
    const int peekSize = device->peek((char *)&payloadSize, sizeof(Protocol::PayloadSize));
    if (peekSize < (int)sizeof(Protocol::PayloadSize))
        return false;

    if (payloadSize == -1 && !device->isSequential()) // input end on shared memory
        return false;

    payloadSize = abs(qFromBigEndian(payloadSize));
    return device->bytesAvailable() >= payloadSize + minimumSize;
}

Message Message::readMessage(QIODevice *device)
{
    Message msg;

    Protocol::PayloadSize payloadSize = readNumber<qint32>(device);

    msg.m_objectAddress = readNumber<Protocol::ObjectAddress>(device);
    msg.m_messageType = readNumber<Protocol::MessageType>(device);
    Q_ASSERT(msg.m_messageType != Protocol::InvalidMessageType);
    Q_ASSERT(msg.m_objectAddress != Protocol::InvalidObjectAddress);
    if (payloadSize < 0) {
        payloadSize = abs(payloadSize);
        auto& uncompressedData = msg.m_buffer->scratchSpace;
        uncompressedData.resize(payloadSize);
        device->read(uncompressedData.data(), payloadSize);
        uncompress(uncompressedData, msg.m_buffer->data.buffer());
        Q_ASSERT(payloadSize == uncompressedData.size());
    } else {
        if (payloadSize > 0) {
            msg.m_buffer->data.buffer() = device->read(payloadSize);
            Q_ASSERT(payloadSize == msg.m_buffer->data.size());
        }
    }

    msg.m_buffer->resetStatus();

    return msg;
}

quint8 Message::lowestSupportedDataVersion()
{
    return QDataStream::Qt_5_5;
}

quint8 Message::highestSupportedDataVersion()
{
    return QDataStream::Qt_DefaultCompiledVersion;
}

quint8 Message::negotiatedDataVersion()
{
    return s_streamVersion;
}

void Message::setNegotiatedDataVersion(quint8 version)
{
    s_streamVersion = version;
}

void Message::resetNegotiatedDataVersion()
{
    s_streamVersion = lowestSupportedDataVersion();
}

void Message::write(QIODevice *device) const
{
    Q_ASSERT(m_objectAddress != Protocol::InvalidObjectAddress);
    Q_ASSERT(m_messageType != Protocol::InvalidMessageType);
    static const bool compressionEnabled = qgetenv("GAMMARAY_DISABLE_LZ4") != "1";
    const int buffSize = m_buffer->data.size();
    auto& compressedData = m_buffer->scratchSpace;
    if (buffSize > minimumUncompressedSize && compressionEnabled)
        compress(m_buffer->data.buffer(), compressedData);

    const bool isCompressed = compressedData.size() && compressedData.size() < buffSize;
    if (isCompressed)
        writeNumber<Protocol::PayloadSize>(device, -compressedData.size()); // send compressed Buffer
    else
        writeNumber<Protocol::PayloadSize>(device, buffSize);   // send uncompressed Buffer

    writeNumber(device, m_objectAddress);
    writeNumber(device, m_messageType);

    if (buffSize) {
        if (isCompressed) {
            const int s = device->write(compressedData);
            Q_ASSERT(s == compressedData.size());
            Q_UNUSED(s);
        } else {
            const int s = device->write(m_buffer->data.buffer());
            Q_ASSERT(s == m_buffer->data.size());
            Q_UNUSED(s);
        }
    }
}

int Message::size() const
{
    return m_buffer->data.size();
}
