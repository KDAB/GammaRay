/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROXYFACTORYBASE_H
#define GAMMARAY_PROXYFACTORYBASE_H

#include <QObject>
#include <QSettings>
#include <QString>

#include <iostream>

namespace GammaRay
{

/** Base class for wrappers for potentially not yet loaded plugins. */
class ProxyFactoryBase : public QObject
{
  Q_OBJECT
public:
  explicit ProxyFactoryBase(const QString &desktopFilePath, QObject *parent = 0);
  ~ProxyFactoryBase();

  QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
  QString errorString() const;

protected:
  void loadPlugin();

  QString m_id;
  QObject *m_factory;
  QString m_errorString;
  QString m_pluginPath;

private:
  QSettings *m_desktopFile;
};

template <typename IFace>
class ProxyFactory : public ProxyFactoryBase, public IFace
{
public:
  explicit inline ProxyFactory(const QString &desktopFilePath, QObject *parent = 0)
    : ProxyFactoryBase(desktopFilePath, parent) {}
  inline ~ProxyFactory() {}

  inline /*override*/ QString id() const
  {
    return m_id;
  }

protected:
  IFace *factory()
  {
    loadPlugin();
    IFace *iface = qobject_cast<IFace*>(m_factory);
    if (!iface) {
      m_errorString =
        QObject::tr("Plugin does not provide an instance of %1.").
        arg(qobject_interface_iid<IFace*>());
      std::cerr << "Failed to cast object from " << qPrintable(m_pluginPath)
                << " to " << qobject_interface_iid<IFace*>();
    }
    return iface;
  }
};

}

#endif // GAMMARAY_PROXYFACTORYBASE_H
