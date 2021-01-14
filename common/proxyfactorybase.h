/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

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

#ifndef GAMMARAY_PROXYFACTORYBASE_H
#define GAMMARAY_PROXYFACTORYBASE_H

#include "plugininfo.h"

#include <QCoreApplication>
#include <QObject>
#include <QString>

#include <iostream>

namespace GammaRay {
/** Base class for wrappers for potentially not yet loaded plugins. */
class ProxyFactoryBase : public QObject
{
    Q_OBJECT
public:
    explicit ProxyFactoryBase(const PluginInfo &pluginInfo, QObject *parent = nullptr);
    ~ProxyFactoryBase() override;

    PluginInfo pluginInfo() const;
    QString errorString() const;

protected:
    void loadPlugin();

    QObject *m_factory;
    QString m_errorString;

private:
    PluginInfo m_pluginInfo;
};

template<typename IFace>
class ProxyFactory : public ProxyFactoryBase, public IFace
{
public:
    explicit inline ProxyFactory(const PluginInfo &pluginInfo, QObject *parent = nullptr)
        : ProxyFactoryBase(pluginInfo, parent) {}
    inline ~ProxyFactory() override = default;

    QString id() const override
    {
        return pluginInfo().id();
    }

protected:
    IFace *factory()
    {
        loadPlugin();
        IFace *iface = qobject_cast<IFace *>(m_factory);
        if (!iface) {
            m_errorString = qApp->translate("GammaRay::ProxyFactory",
                                            "Plugin does not provide an instance of %1.")
                                                .arg(qobject_interface_iid<IFace *>());
            std::cerr << "Failed to cast object from " << qPrintable(pluginInfo().path())
                      << " to " << qobject_interface_iid<IFace *>() << std::endl;
        }
        return iface;
    }
};
}

#endif // GAMMARAY_PROXYFACTORYBASE_H
