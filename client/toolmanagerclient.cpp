/*
  toolmanagerclient.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "toolmanagerclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ToolManagerClient::ToolManagerClient(QObject *parent)
    : ToolManagerInterface(parent)
{
}

void ToolManagerClient::selectObject(const ObjectId &id, const QString &toolId)
{
    Endpoint::instance()->invokeObject(objectName(), "selectObject",
                                       QVariantList() << QVariant::fromValue(id) << toolId);
}

void ToolManagerClient::requestToolsForObject(const ObjectId &id)
{
    Endpoint::instance()->invokeObject(objectName(), "requestToolsForObject",
                                       QVariantList() << QVariant::fromValue(id));
}

void ToolManagerClient::requestAvailableTools()
{
    Endpoint::instance()->invokeObject(objectName(), "requestAvailableTools");
}
