/*
  widgetinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
