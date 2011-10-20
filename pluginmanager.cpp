#include "pluginmanager.h"

#include "config-gammaray.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QLibrary>

#include <QPluginLoader>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

static const QLatin1String GAMMARAY_PLUGIN_SUFFIX("gammaray");
PluginManager* PluginManager::s_instance = 0;

PluginManager* PluginManager::instance()
{
    if (!s_instance)
        s_instance = new PluginManager();
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
    QDir::separator() + "plugins";

  QStringList libraryPaths = QCoreApplication::libraryPaths();
  foreach (const QString& libraryPath, libraryPaths) {
    pluginPaths << libraryPath + QDir::separator() + GAMMARAY_PLUGIN_SUFFIX;
  }

  return pluginPaths;
}

QStringList PluginManager::plugins() const
{
  // TODO: temporary work-around to avoid loading the same plugin two times
  // should be fixed when providing valid plugin spec files
  static QStringList loadedPluginNames;

  QStringList r;
  foreach (const QString &pluginPath, pluginPaths()) {
    const QDir dir(pluginPath);
    IF_DEBUG(cout << "checking plugin path: " << qPrintable(dir.absolutePath()) << endl);
    foreach (const QString &plugin, dir.entryList(QDir::Files)) {
      const QString pluginFile = dir.absoluteFilePath(plugin);
      const QString pluginName = QFileInfo(pluginFile).baseName();

      if (loadedPluginNames.contains(pluginName)) {
        cout << "not loading plugin, already loaded: " << qPrintable(pluginFile) << endl;
        continue;
      }

      if (QLibrary::isLibrary(pluginFile)) {
        cout << "loading plugin" << qPrintable(pluginFile) << endl;
        r.push_back(pluginFile);
        loadedPluginNames << pluginName;
      }
    }
  }
  return r;
}
