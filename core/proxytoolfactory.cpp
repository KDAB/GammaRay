/*
  proxytoolfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "proxytoolfactory.h"

using namespace GammaRay;

ProxyToolFactory::ProxyToolFactory(const PluginInfo &pluginInfo, QObject *parent)
    : ProxyFactory<ToolFactory>(pluginInfo, parent)
{
    const QStringList typesList = pluginInfo.supportedTypes();
    QVector<QByteArray> typesVector;
    typesVector.reserve(typesList.size());
    for (auto it = typesList.constBegin(), end = typesList.constEnd(); it != end; ++it)
        typesVector << (*it).toLatin1();
    setSupportedTypes(typesVector);
}

bool ProxyToolFactory::isValid() const
{
    return pluginInfo().isValid()
        && !id().isEmpty()
        && !supportedTypes().isEmpty();
}

void ProxyToolFactory::init(Probe *probe)
{
    loadPlugin();
    ToolFactory *fac = factory();
    if (!fac)
        return;
    Q_ASSERT(fac);
    fac->init(probe);
}

bool ProxyToolFactory::isHidden() const
{
    return pluginInfo().isHidden();
}

QVector<QByteArray> ProxyToolFactory::selectableTypes() const
{
    return pluginInfo().selectableTypes();
}
