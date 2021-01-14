/*
  styleinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "styleinspectorinterface.h"
#include <common/objectbroker.h>
#include <QSize>

using namespace GammaRay;

StyleInspectorInterface::StyleInspectorInterface(QObject *parent)
    : QObject(parent)
    , m_cellHeight(64)
    , m_cellWidth(64)
    , m_cellZoom(1)
{
    ObjectBroker::registerObject<StyleInspectorInterface *>(this);
}

StyleInspectorInterface::~StyleInspectorInterface() = default;

int StyleInspectorInterface::cellHeight() const
{
    return m_cellHeight;
}

int StyleInspectorInterface::cellWidth() const
{
    return m_cellWidth;
}

int StyleInspectorInterface::cellZoom() const
{
    return m_cellZoom;
}

QSize StyleInspectorInterface::cellSizeHint() const
{
    return {m_cellWidth * m_cellZoom, m_cellHeight * m_cellZoom};
}

void StyleInspectorInterface::setCellHeight(int height)
{
    m_cellHeight = height;
    emit cellSizeChanged();
}

void StyleInspectorInterface::setCellWidth(int width)
{
    m_cellWidth = width;
    emit cellSizeChanged();
}

void StyleInspectorInterface::setCellZoom(int zoom)
{
    m_cellZoom = zoom;
    emit cellSizeChanged();
}
