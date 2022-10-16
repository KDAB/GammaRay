/*
  remoteviewframe.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_REMOTEVIEWFRAME_H
#define GAMMARAY_REMOTEVIEWFRAME_H

#include "gammaray_common_export.h"
#include "transferimage.h"

#include <QDataStream>
#include <QImage>
#include <QMetaType>
#include <QVariant>

namespace GammaRay {
class RemoteViewFrame;

GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &stream, const GammaRay::RemoteViewFrame &frame);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &stream, GammaRay::RemoteViewFrame &frame);

/** Data of a single frame displayed in the RemoteViewWidget. */
class GAMMARAY_COMMON_EXPORT RemoteViewFrame
{
public:
    RemoteViewFrame() = default;
    ~RemoteViewFrame();

    bool isValid() const;

    /// the visible area on screen
    QRectF viewRect() const;
    void setViewRect(const QRectF &viewRect);
    /// the internal scene might expand beyond the visible view area
    QRectF sceneRect() const;
    void setSceneRect(const QRectF &sceneRect);

    QImage image() const;
    QTransform transform() const;
    void setImage(const QImage &image);
    void setImage(const QImage &image, const QTransform &transform);

    /// tool specific frame data
    QVariant data() const;
    void setData(const QVariant &data);

private:
    friend QDataStream &operator<<(QDataStream &stream, const RemoteViewFrame &frame);
    friend QDataStream &operator>>(QDataStream &stream, RemoteViewFrame &frame);
    TransferImage m_image;
    QVariant m_data;
    QRectF m_viewRect;
    QRectF m_sceneRect;
};
}

Q_DECLARE_METATYPE(GammaRay::RemoteViewFrame)

#endif // GAMMARAY_REMOTEVIEWFRAME_H
