/*
  tooluifactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
