/*
  styleinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    return { m_cellWidth * m_cellZoom, m_cellHeight * m_cellZoom };
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
