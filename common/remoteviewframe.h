/*
  remoteviewframe.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

QDataStream &operator<<(QDataStream &stream, const GammaRay::RemoteViewFrame &frame);
QDataStream &operator>>(QDataStream &stream, GammaRay::RemoteViewFrame &frame);

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
    /// the interal scene might expand beyond the visible view area
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
