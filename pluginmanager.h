/*
  pluginmanager.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
