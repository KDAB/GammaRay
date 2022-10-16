/*
  widgetinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "widgetinspectorinterface.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>

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
    StreamOperators::registerOperators<Features>();
    StreamOperators::registerOperators<WidgetFrameData>();

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

QDataStream &operator<<(QDataStream &out, const WidgetFrameData &data)
{
    out << data.tabFocusRects;
    return out;
}

QDataStream &operator>>(QDataStream &in, WidgetFrameData &data)
{
    in >> data.tabFocusRects;
    return in;
}

}
