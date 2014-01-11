/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "proxyfactorybase.h"

#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QPluginLoader>

#include <iostream>

using namespace GammaRay;

ProxyFactoryBase::ProxyFactoryBase(const QString& desktopFilePath, QObject* parent):
  QObject(parent),
  m_factory(0),
  m_desktopFile(0)
{
  const QFileInfo pluginInfo(desktopFilePath);
  m_desktopFile = new QSettings(desktopFilePath, QSettings::IniFormat);
  m_desktopFile->beginGroup(QLatin1String("Desktop Entry"));

  m_id = value("X-GammaRay-Id", pluginInfo.baseName()).toString();

  const QString dllBaseName = value(QLatin1String("Exec")).toString();
  if (dllBaseName.isEmpty()) {
    m_errorString = tr("Invalid 'Exec' line in plugin spec file");
    return;
  }

  foreach (const QString &entry, pluginInfo.dir().entryList(QStringList(dllBaseName + QLatin1Char('*')), QDir::Files)) {
    const QString path = pluginInfo.dir().absoluteFilePath(entry);
    if (QLibrary::isLibrary(path)) {
      m_pluginPath = path;
      break;
    }
  }

}

ProxyFactoryBase::~ProxyFactoryBase()
{

}

QVariant ProxyFactoryBase::value(const QString& key, const QVariant &defaultValue) const
{
  return m_desktopFile->value(key, defaultValue);
}


QString ProxyFactoryBase::errorString() const
{
  return m_errorString;
}

void ProxyFactoryBase::loadPlugin()
{
  if (m_factory)
    return;
  QPluginLoader loader(m_pluginPath, this);
  m_factory = loader.instance();
  if (m_factory) {
    m_factory->setParent(this);
  } else {
    m_errorString = loader.errorString();
    std::cerr << "error loading plugin " << qPrintable(m_pluginPath)
              << ": " << qPrintable(loader.errorString()) << std::endl;
  }
}
