/*
  pluginmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "proxytoolfactory.h"

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
PluginManager *PluginManager::s_instance = 0;

PluginManager *PluginManager::instance()
{
  if (!s_instance) {
    s_instance = new PluginManager();
    s_instance->scan();
  }
  return s_instance;
}

PluginManager::PluginManager()
{
  QCoreApplication::addLibraryPath(QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR));
}

QStringList PluginManager::pluginPaths() const
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

void PluginManager::scan()
{
  m_errors.clear();
  QStringList loadedPluginNames;

  foreach (const QString &pluginPath, pluginPaths()) {
    const QDir dir(pluginPath);
    IF_DEBUG(cout << "checking plugin path: " << qPrintable(dir.absolutePath()) << endl);
    foreach (const QString &plugin, dir.entryList(QDir::Files)) {
      const QString pluginFile = dir.absoluteFilePath(plugin);
      const QFileInfo pluginInfo(pluginFile);
      const QString pluginName = pluginInfo.baseName();

      if (loadedPluginNames.contains(pluginName)) {
        cout << "not loading plugin, already loaded: " << qPrintable(pluginFile) << endl;
        continue;
      }

      if (pluginInfo.suffix() == QLatin1String("desktop")) {
        ProxyToolFactory *proxy = new ProxyToolFactory(pluginFile);
        if (!proxy->isValid()) {
          m_errors << PluginLoadError(pluginFile, QObject::tr("Failed to load plugin."));
          std::cerr << "invalid plugin " << qPrintable(pluginFile) << std::endl;
          delete proxy;
        } else {
          IF_DEBUG(cout << "plugin looks valid " << qPrintable(pluginFile) << endl;)
          m_plugins.push_back(proxy);
          loadedPluginNames << pluginName;
        }
      }
    }
  }
}

QVector<ToolFactory *> PluginManager::plugins()
{
  return m_plugins;
}
