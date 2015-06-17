/*
  pluginmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

PluginManagerBase::PluginManagerBase(QObject *parent) : m_parent(parent)
{
}

PluginManagerBase::~PluginManagerBase()
{
}

QStringList PluginManagerBase::pluginPaths() const
{
  QStringList pluginPaths;
  pluginPaths.push_back(Paths::currentProbePath());
  return pluginPaths;
}

QStringList PluginManagerBase::pluginFilter() const
{
  QStringList filter;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  filter.push_back("*.desktop");
#elif defined(Q_OS_ANDROID)
  filter.push_back(QLatin1String("libplugins_gammaray_gammaray_*") + Paths::pluginExtension());
#else
  filter.push_back(QLatin1String("*") + Paths::pluginExtension());
#endif
  return filter;
}

void PluginManagerBase::scan(const QString &serviceType)
{
  m_errors.clear();
  QStringList loadedPluginNames;

  foreach (const QString &pluginPath, pluginPaths()) {
    const QDir dir(pluginPath);
    IF_DEBUG(cout << "checking plugin path: " << qPrintable(dir.absolutePath()) << endl);
    foreach (const QString &plugin, dir.entryList(pluginFilter(), QDir::Files)) {
      const QString pluginFile = dir.absoluteFilePath(plugin);
      const PluginInfo pluginInfo(pluginFile);

      if (!pluginInfo.isValid() || loadedPluginNames.contains(pluginInfo.id())) {
        continue;
      }

      if (pluginInfo.interface() != serviceType) {
        IF_DEBUG(qDebug() << Q_FUNC_INFO << "skipping" << pluginFile << "not supporting service type" << serviceType << "service types are: " << pluginInfo.interface();)
        continue;
      }

      if (createProxyFactory(pluginInfo, m_parent))
        loadedPluginNames.push_back(pluginInfo.id());
    }
  }
}
