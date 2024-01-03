/*
  networksupportclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Shantanu Tushar <shantanu.tushar@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networksupportclient.h"

namespace GammaRay {
NetworkSupportClient::NetworkSupportClient(QObject *parent)
    : NetworkSupportInterface(parent)
{
}

NetworkSupportClient::~NetworkSupportClient() = default;
}
