/*
  remoteviewframe.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

using namespace GammaRay;

RemoteViewFrame::RemoteViewFrame()
{
}

RemoteViewFrame::~RemoteViewFrame()
{
}

bool RemoteViewFrame::isValid() const
{
    return !m_image.image().isNull();
}

QSize RemoteViewFrame::size() const
{
    return m_image.image().size();
}

int RemoteViewFrame::width() const
{
    return m_image.image().width();
}

int RemoteViewFrame::height() const
{
    return m_image.image().height();
}

QImage RemoteViewFrame::image() const
{
    return m_image.image();
}

void RemoteViewFrame::setImage(const QImage& image)
{
    m_image.setImage(image);
}

QVariant RemoteViewFrame::data() const
{
    return m_data;
}

void RemoteViewFrame::setData(const QVariant& data)
{
    m_data = data;
}

QDataStream& operator<<(QDataStream& stream, const RemoteViewFrame& frame)
{
    stream << frame.m_image << frame.m_data;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, RemoteViewFrame& frame)
{
    stream >> frame.m_image;
    stream >> frame.m_data;
    return stream;
}
