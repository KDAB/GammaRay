/*
  connectionsextensioninterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "connectionsextensioninterface.h"

#include <common/objectbroker.h>

using namespace GammaRay;

ConnectionsExtensionInterface::ConnectionsExtensionInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    ObjectBroker::registerObject(name, this);
}

ConnectionsExtensionInterface::~ConnectionsExtensionInterface() = default;

QString ConnectionsExtensionInterface::name() const
{
    return m_name;
}
