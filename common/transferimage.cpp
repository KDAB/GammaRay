/*
  transferimage.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "transferimage.h"

#include <QDebug>
#include <QIODevice>

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
    stream << ( quint32 )(format);
    switch (format) {
    case TransferImage::QImageFormat:
        stream << img;
        break;
    case TransferImage::RawFormat:
        stream << ( double )img.devicePixelRatio();
        stream << ( quint32 )img.format() << ( quint32 )img.width() << ( quint32 )img.height() << image.transform();
        stream.device()->write(( const char * )img.constBits(), img.sizeInBytes());
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
    case TransferImage::QImageFormat: {
        QImage img;
        stream >> img;
        image.setImage(img);
        break;
    }
    case TransferImage::RawFormat: {
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
