/*
  materialextensioninterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "materialextensioninterface.h"
#include <common/objectbroker.h>

using namespace GammaRay;

MaterialExtensionInterface::MaterialExtensionInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    ObjectBroker::registerObject(name, this);
}

MaterialExtensionInterface::~MaterialExtensionInterface() = default;

const QString &MaterialExtensionInterface::name() const
{
    return m_name;
}
