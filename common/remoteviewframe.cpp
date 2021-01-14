/*
  remoteviewframe.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remoteviewframe.h"

#include <QDataStream>

namespace GammaRay {
RemoteViewFrame::~RemoteViewFrame() = default;

bool RemoteViewFrame::isValid() const
{
    return !m_image.image().isNull();
}

QRectF RemoteViewFrame::viewRect() const
{
    if (m_viewRect.isValid())
        return m_viewRect;
    qreal pxRatio = 1.0;
    pxRatio = m_image.image().devicePixelRatio();
    return QRect(QPoint(), m_image.image().size() / pxRatio);
}

void RemoteViewFrame::setViewRect(const QRectF &viewRect)
{
    m_viewRect = viewRect;
}

QRectF RemoteViewFrame::sceneRect() const
{
    if (m_sceneRect.isValid())
        return m_sceneRect;
    return viewRect();
}

void RemoteViewFrame::setSceneRect(const QRectF &sceneRect)
{
    m_sceneRect = sceneRect;
}

QImage RemoteViewFrame::image() const
{
    return m_image.image();
}

QTransform RemoteViewFrame::transform() const
{
    return m_image.transform();
}

void RemoteViewFrame::setImage(const QImage &image)
{
    m_image.setImage(image);
}

void RemoteViewFrame::setImage(const QImage &image, const QTransform &transform)
{
    m_image.setImage(image);
    m_image.setTransform(transform);
}

QVariant RemoteViewFrame::data() const
{
    return m_data;
}

void RemoteViewFrame::setData(const QVariant &data)
{
    m_data = data;
}

QDataStream &operator<<(QDataStream &stream, const RemoteViewFrame &frame)
{
    stream << frame.m_image << frame.m_data << frame.m_viewRect << frame.m_sceneRect;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, RemoteViewFrame &frame)
{
    stream >> frame.m_image;
    stream >> frame.m_data;
    stream >> frame.m_viewRect;
    stream >> frame.m_sceneRect;
    return stream;
}
}
