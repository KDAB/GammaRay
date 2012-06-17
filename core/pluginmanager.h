/*
  pluginmanager.h

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

#ifndef GAMMARAY_PLUGINMANAGER_H
#define GAMMARAY_PLUGINMANAGER_H

#include <QVector>
#include <QList>
#include <QFileInfo>

namespace GammaRay {

class ToolFactory;
class PluginLoadError;

typedef QList<PluginLoadError> PluginLoadErrors;

class PluginLoadError
{
  public:
    PluginLoadError(QString _pluginFile, QString _errorString)
      : pluginFile(_pluginFile), errorString(_errorString) {}

    QString pluginName() const { return QFileInfo(pluginFile).baseName(); }

  public:
    QString pluginFile;
    QString errorString;
};

class PluginManager
{
  public:
    static PluginManager *instance();

    QVector<ToolFactory*> plugins();

    QList<PluginLoadError> errors() { return m_errors; }

  protected:
    PluginManager();

  private:
    QStringList pluginPaths() const;
    void scan();

    static PluginManager *s_instance;
    QVector<ToolFactory*> m_plugins;
    QList<PluginLoadError> m_errors;
};

}

#endif // GAMMARAY_PLUGINMANAGER_H
