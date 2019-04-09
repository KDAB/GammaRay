/*
  transferimage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "transferimage.h"

#include <QDebug>

namespace GammaRay {
TransferImage::TransferImage(const QImage &image)
    : m_image(image)
{
}

const QImage &TransferImage::image() const
{
    return m_image;
}

void TransferImage::setImage(const QImage &image)
{
    m_image = image;
}

QTransform TransferImage::transform() const
{
    return m_transform;
}

void TransferImage::setTransform(const QTransform &transform)
{
    m_transform = transform;
}

QDataStream &operator<<(QDataStream &stream, const GammaRay::TransferImage &image)
{
    static const TransferImage::Format format = TransferImage::RawFormat;

    const QImage &img = image.image();
    stream << (quint32)(format);
    switch (format) {
    case TransferImage::QImageFormat:
        stream << img;
        break;
    case TransferImage::RawFormat:
        stream << (double)img.devicePixelRatio();
        stream << (quint32)img.format() << (quint32)img.width() << (quint32)img.height() << image.transform();
        stream.device()->write((const char*)img.constBits(), img.byteCount());
        break;
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, TransferImage &image)
{
    quint32 i;
    stream >> i;
    const TransferImage::Format format = static_cast<TransferImage::Format>(i);

    switch (format) {
    case TransferImage::QImageFormat:
    {
        QImage img;
        stream >> img;
        image.setImage(img);
        break;
    }
    case TransferImage::RawFormat:
    {
        double r;
        quint32 f, w, h;
        QTransform transform;
        stream >> r >> f >> w >> h >> transform;
        QImage img(w, h, static_cast<QImage::Format>(f));
        img.setDevicePixelRatio(r);
        for (int i = 0; i < img.height(); ++i) {
            const QByteArray buffer = stream.device()->read(img.bytesPerLine());
            memcpy(img.scanLine(i), buffer.constData(), img.bytesPerLine());
        }

        image.setImage(img);
        image.setTransform(transform);
        break;
    }
    }

    return stream;
}
}
