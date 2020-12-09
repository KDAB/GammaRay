/*
  quickinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspectorinterface.h"
#include "quickitemmodelroles.h"
#include "quickdecorationsdrawer.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>

#include <QDataStream>

using namespace GammaRay;

namespace GammaRay {
QDataStream &operator<<(QDataStream &out, QuickInspectorInterface::Features value)
{
    out << qint32(value);
    return out;
}

QDataStream &operator>>(QDataStream &in, QuickInspectorInterface::Features &value)
{
    qint32 t;
    in >> t;
    value = static_cast<QuickInspectorInterface::Features>(t);
    return in;
}

QDataStream &operator<<(QDataStream &out, QuickInspectorInterface::RenderMode value)
{
    out << qint32(value);
    return out;
}

QDataStream &operator>>(QDataStream &in, QuickInspectorInterface::RenderMode &value)
{
    qint32 t;
    in >> t;
    value = static_cast<QuickInspectorInterface::RenderMode>(t);
    return in;
}
}

QuickInspectorInterface::QuickInspectorInterface(QObject *parent)
    : QObject(parent)
    , m_serverSideDecoration(false)
{
    ObjectBroker::registerObject<QuickInspectorInterface *>(this);
    StreamOperators::registerOperators<Features>();
    StreamOperators::registerOperators<RenderMode>();
    StreamOperators::registerOperators<QuickItemGeometry>();
    StreamOperators::registerOperators<QVector<QuickItemGeometry>>();
    StreamOperators::registerOperators<QuickDecorationsSettings>();
}

QuickInspectorInterface::~QuickInspectorInterface() = default;

bool QuickInspectorInterface::serverSideDecorationEnabled() const
{
    return m_serverSideDecoration;
}

void QuickInspectorInterface::setServerSideDecorationsEnabled(bool enabled)
{
    if (m_serverSideDecoration == enabled)
        return;
    m_serverSideDecoration = enabled;
    emit serverSideDecorationChanged(enabled);
}
