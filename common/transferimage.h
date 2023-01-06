/*
  transferimage.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TRANSFERIMAGE_H
#define GAMMARAY_TRANSFERIMAGE_H

#include <QDataStream>
#include <QImage>
#include <QVariant>

namespace GammaRay {
/** Wrapper class for a QImage to allow raw data transfer over a QDataStream, bypassing the usuale PNG encoding. */
class TransferImage
{
public:
    TransferImage() = default;
    explicit TransferImage(const QImage &image);

    const QImage &image() const;
    void setImage(const QImage &image);

    QTransform transform() const;
    void setTransform(const QTransform &transform);

    enum Format
    {
        QImageFormat,
        RawFormat
    };

private:
    QImage m_image;
    QTransform m_transform;
};

QDataStream &operator<<(QDataStream &stream, const GammaRay::TransferImage &image);
QDataStream &operator>>(QDataStream &stream, GammaRay::TransferImage &image);
}

Q_DECLARE_METATYPE(GammaRay::TransferImage)

#endif // GAMMARAY_TRANSFERIMAGE_H
