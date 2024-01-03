/*
  tooluifactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "tooluifactory.h"

using namespace GammaRay;

ToolUiFactory::ToolUiFactory() = default;

ToolUiFactory::~ToolUiFactory() = default;

QString ToolUiFactory::name() const
{
    return QString(); // in the common case this is provided via ProxyToolUIFactory
}

bool ToolUiFactory::remotingSupported() const
{
    return true;
}

void ToolUiFactory::initUi()
{
}
