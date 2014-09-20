/*
  transferimage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

using namespace GammaRay;

TransferImage::TransferImage()
{
}

TransferImage::TransferImage(const QImage& image) : m_image(image)
{
}

QImage TransferImage::image() const
{
    return m_image;
}

void TransferImage::setImage(const QImage& image)
{
    m_image = image;
}


QDataStream& operator<<(QDataStream& stream, const GammaRay::TransferImage& image)
{
    static const TransferImage::Format format = TransferImage::RawFormat;

    const QImage &img = image.image();
    stream << (quint32)(format);
    switch (format) {
        case TransferImage::QImageFormat:
            stream << img;
            break;
        case TransferImage::RawFormat:
        {
            stream << (quint32)img.format() << (quint32)img.width() << (quint32)img.height();
            for (int i = 0; i < img.height(); ++i) {
              stream.device()->write((const char*)img.scanLine(i), img.bytesPerLine());
            }
            break;
        }
    }

    return stream;
}

QDataStream& operator>>(QDataStream& stream, TransferImage& image)
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
            quint32 f, w, h;
            stream >> f >> w >> h;
            QImage img(w, h, static_cast<QImage::Format>(f));
            for (int i = 0; i < img.height(); ++i) {
              const QByteArray buffer = stream.device()->read(img.bytesPerLine());
              qMemCopy(img.scanLine(i), buffer.constData(), img.bytesPerLine());
            }
            image.setImage(img);
            break;
        }
    }

    return stream;
}
