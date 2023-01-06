/*
  toolmanagerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "toolmanagerinterface.h"
#include "streamoperators.h"

using namespace GammaRay;

ToolManagerInterface::ToolManagerInterface(QObject *parent)
    : QObject(parent)
{
    StreamOperators::registerOperators<ToolData>();
    StreamOperators::registerOperators<QVector<ToolData>>();
    StreamOperators::registerOperators<QVector<QString>>();
}

ToolManagerInterface::~ToolManagerInterface() = default;
