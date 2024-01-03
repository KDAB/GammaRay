/*
  networksupportinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Shantanu Tushar <shantanu.tushar@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networksupportinterface.h"

#include <common/objectbroker.h>

using namespace GammaRay;

NetworkSupportInterface::NetworkSupportInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<NetworkSupportInterface *>(this);
}

NetworkSupportInterface::~NetworkSupportInterface() = default;
