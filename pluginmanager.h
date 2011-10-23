#ifndef GAMMARAY_PLUGINMANAGER_H
#define GAMMARAY_PLUGINMANAGER_H

#include <QStringList>
#include <QPluginLoader>

#include <iostream>

using namespace std;

namespace GammaRay {

class PluginManager
{
  public:
    static PluginManager *instance();

  protected:
    PluginManager();

  public:
    template<typename T>
    QList<T*> allObjects() const
    {
      QList<T*> objects;
      foreach (const QString &pluginFile, plugins()) {
        QPluginLoader loader(pluginFile);
        if (loader.load()) {
          T* object = qobject_cast<T*>(loader.instance());
          if (object) {
            objects << object;
          }
        } else {
          cout << "error loading plugin: " << qPrintable(loader.errorString()) << endl;
        }
      }
      return objects;
    }

    QStringList pluginPaths() const;
    QStringList plugins() const;

  private:
    static PluginManager *s_instance;
};

}

#endif // GAMMARAY_PLUGINMANAGER_H
