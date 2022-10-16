/*
  proxytooluifactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "proxytooluifactory.h"

#include <QLabel>

using namespace GammaRay;

ProxyToolUiFactory::ProxyToolUiFactory(const PluginInfo &pluginInfo, QObject *parent)
    : ProxyFactory<ToolUiFactory>(pluginInfo, parent)
{
}

QString ProxyToolUiFactory::name() const
{
    return pluginInfo().name();
}

bool ProxyToolUiFactory::isValid() const
{
    return pluginInfo().isValid();
}

bool ProxyToolUiFactory::remotingSupported() const
{
    return pluginInfo().remoteSupport();
}

QWidget *ProxyToolUiFactory::createWidget(QWidget *parentWidget)
{
    loadPlugin();
    ToolUiFactory *fac = factory();
    if (!fac)
        return new QLabel(tr("Plugin '%1' could not be loaded.").arg(pluginInfo().path()), parentWidget);
    Q_ASSERT(fac);
    return fac->createWidget(parentWidget);
}

void ProxyToolUiFactory::initUi()
{
    loadPlugin();
    ToolUiFactory *fac = factory();
    if (!fac)
        return;
    fac->initUi();
}
