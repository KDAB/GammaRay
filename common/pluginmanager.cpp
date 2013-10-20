/*
  pluginmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "config-gammaray.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>

#include <iostream>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

static const QLatin1String GAMMARAY_PLUGIN_SUFFIX("gammaray");

PluginManagerBase::PluginManagerBase(QObject *parent) : m_parent(parent)
{
  QCoreApplication::addLibraryPath(QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR));
}

PluginManagerBase::~PluginManagerBase()
{
}

QStringList PluginManagerBase::pluginPaths() const
{
  QStringList pluginPaths;

  // add plugins from gammaray's build directory
  pluginPaths << QLatin1String(GAMMARAY_BUILD_DIR) +
    QDir::separator() + "lib" +
    QDir::separator() + "plugins" +
    QDir::separator() + GAMMARAY_PLUGIN_SUFFIX;

  QStringList libraryPaths = QCoreApplication::libraryPaths();
  foreach (const QString &libraryPath, libraryPaths) {
    pluginPaths << libraryPath + QDir::separator() + GAMMARAY_PLUGIN_SUFFIX;
  }

  return pluginPaths;
}

void PluginManagerBase::scan()
{
  m_errors.clear();
  QStringList loadedPluginNames;

  foreach (const QString &pluginPath, pluginPaths()) {
    const QDir dir(pluginPath);
    IF_DEBUG(cout << "checking plugin path: " << qPrintable(dir.absolutePath()) << endl);
    foreach (const QString &plugin, dir.entryList(QStringList() << "*.desktop", QDir::Files)) {
      const QString pluginFile = dir.absoluteFilePath(plugin);
      const QFileInfo pluginInfo(pluginFile);
      const QString pluginName = pluginInfo.baseName();

      if (loadedPluginNames.contains(pluginName)) {
        continue;
      }

      if (createProxyFactory(pluginFile, m_parent))
        loadedPluginNames.push_back(pluginName);
    }
  }
}
