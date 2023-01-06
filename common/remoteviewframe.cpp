/*
  remoteviewframe.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
