/*
  widgetinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "widgetinspectorinterface.h"

#include <common/objectbroker.h>

#include <QDataStream>
#include <QMetaType>

namespace GammaRay {
QDataStream &operator<<(QDataStream &out, WidgetInspectorInterface::Features value)
{
    out << qint32(value);
    return out;
}

QDataStream &operator>>(QDataStream &in, WidgetInspectorInterface::Features &value)
{
    qint32 t;
    in >> t;
    value = static_cast<WidgetInspectorInterface::Features>(t);
    return in;
}

WidgetInspectorInterface::WidgetInspectorInterface(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaTypeStreamOperators<Features>();
    qRegisterMetaTypeStreamOperators<WidgetFrameData>();
    ObjectBroker::registerObject<WidgetInspectorInterface *>(this);
}

WidgetInspectorInterface::~WidgetInspectorInterface() = default;

WidgetInspectorInterface::Features WidgetInspectorInterface::features() const
{
    return m_features;
}

void WidgetInspectorInterface::setFeatures(WidgetInspectorInterface::Features features)
{
    if (features == m_features)
        return;
    m_features = features;
    emit featuresChanged();
}

QDataStream& operator<<(QDataStream &out, const WidgetFrameData &data)
{
    out << data.tabFocusRects;
    return out;
}

QDataStream& operator>>(QDataStream& in, WidgetFrameData &data)
{
    in >> data.tabFocusRects;
    return in;
}

}
