/*
  messagehandlerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "messagehandlerinterface.h"
#include "common/objectbroker.h"

using namespace GammaRay;

MessageHandlerInterface::MessageHandlerInterface(QObject *parent)
    : QObject(parent)
    , m_stackTraceAvailable(false)
{
    ObjectBroker::registerObject<MessageHandlerInterface *>(this);
}

MessageHandlerInterface::~MessageHandlerInterface() = default;

bool MessageHandlerInterface::stackTraceAvailable() const
{
    return m_stackTraceAvailable;
}

void MessageHandlerInterface::setStackTraceAvailable(bool available)
{
    if (m_stackTraceAvailable == available)
        return;
    m_stackTraceAvailable = available;
    emit stackTraceAvailableChanged(available);
}

QStringList MessageHandlerInterface::fullTrace() const
{
    return m_fullTrace;
}

void MessageHandlerInterface::setFullTrace(const QStringList &newFullTrace)
{
    if (m_fullTrace == newFullTrace)
        return;
    m_fullTrace = newFullTrace;
    emit fullTraceChanged();
}
