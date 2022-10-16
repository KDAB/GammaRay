/*
  pluginmanager.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include "pluginmanager.h"
#include "paths.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

#include <iostream>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

PluginManagerBase::PluginManagerBase(QObject *parent)
    : m_parent(parent)
{
}

PluginManagerBase::~PluginManagerBase() = default;

QStringList PluginManagerBase::pluginPaths()
{
#ifndef GAMMARAY_STATIC_PROBE
    return Paths::pluginPaths(GAMMARAY_PROBE_ABI);
#else
    return QStringList();
#endif
}

QStringList PluginManagerBase::pluginFilter()
{
    QStringList filter;
#if defined(GAMMARAY_INSTALL_QT_LAYOUT)
    filter.push_back(QStringLiteral("*") + QStringLiteral(GAMMARAY_PROBE_ABI) + Paths::pluginExtension());
#else
    filter.push_back(QStringLiteral("*") + Paths::pluginExtension());
#endif
    return filter;
}

void PluginManagerBase::scan(const QString &serviceType)
{
    m_errors.clear();
    QStringList loadedPluginNames;

    foreach (const auto &staticPlugin, QPluginLoader::staticPlugins()) {
        PluginInfo pluginInfo(staticPlugin);

        if (!pluginInfo.isValid() || loadedPluginNames.contains(pluginInfo.id()) || pluginInfo.interfaceId() != serviceType) {
            qDebug() << "skipping static plugin " << pluginInfo.id() << pluginInfo.interfaceId();
            continue;
        }

        if (createProxyFactory(pluginInfo, m_parent))
            loadedPluginNames.push_back(pluginInfo.id());
    }

    foreach (const QString &pluginPath, pluginPaths()) {
        const QDir dir(pluginPath);
        IF_DEBUG(cout << "checking plugin path: " << qPrintable(dir.absolutePath()) << endl);
        foreach (const QString &plugin, dir.entryList(pluginFilter(), QDir::Files)) {
            const QString pluginFile = dir.absoluteFilePath(plugin);
            const PluginInfo pluginInfo(pluginFile);

            if (!pluginInfo.isValid() || loadedPluginNames.contains(pluginInfo.id()))
                continue;

            if (pluginInfo.interfaceId() != serviceType) {
                IF_DEBUG(
                    qDebug() << Q_FUNC_INFO << "skipping" << pluginFile << "not supporting service type" << serviceType << "service types are: " << pluginInfo.interfaceId();)
                continue;
            }

            if (createProxyFactory(pluginInfo, m_parent))
                loadedPluginNames.push_back(pluginInfo.id());
        }
    }
}
