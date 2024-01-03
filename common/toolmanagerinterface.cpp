/*
  toolmanagerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
