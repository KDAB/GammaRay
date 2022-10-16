/*
  proxyfactorybase.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
        : ProxyFactoryBase(pluginInfo, parent)
    {
    }
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
