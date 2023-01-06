/*
  quickinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
