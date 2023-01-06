/*
  pluginmanager.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PLUGINMANAGER_H
#define GAMMARAY_PLUGINMANAGER_H

#include "plugininfo.h"

#include <QCoreApplication>
#include <QVector>
#include <QList>
#include <QFileInfo>
#include <QStringList>

#include <iostream>

namespace GammaRay {
class PluginLoadError;

typedef QList<PluginLoadError> PluginLoadErrors;

class PluginLoadError
{
public:
    PluginLoadError(const QString &_pluginFile, const QString &_errorString)
        : pluginFile(_pluginFile)
        , errorString(_errorString)
    {
    }

    QString pluginName() const
    {
        return QFileInfo(pluginFile).baseName();
    }

public:
    QString pluginFile;
    QString errorString;
};

class PluginManagerBase
{
public:
    /**
     * @param parent This is the parent object for all objects created by the plugins
     */
    explicit PluginManagerBase(QObject *parent = nullptr);
    virtual ~PluginManagerBase();

    QList<PluginLoadError> errors() const
    {
        return m_errors;
    }

protected:
    virtual bool createProxyFactory(const PluginInfo &pluginInfo, QObject *parent) = 0;

    void scan(const QString &serviceType);
    static QStringList pluginPaths();
    static QStringList pluginFilter();

    QList<PluginLoadError> m_errors;
    QObject *m_parent;

private:
    Q_DISABLE_COPY(PluginManagerBase)
};

template<typename IFace, typename Proxy>
class PluginManager : public PluginManagerBase
{
public:
    explicit inline PluginManager(QObject *parent = nullptr)
        : PluginManagerBase(parent)
    {
        const QString iid = QString::fromLatin1(qobject_interface_iid<IFace *>());
        Q_ASSERT(!iid.isEmpty());
        const QString serviceType = iid.split(QLatin1Char('/')).first();
        scan(serviceType);
    }

    inline ~PluginManager()
    {
    }

    inline QVector<IFace *> plugins()
    {
        return m_plugins;
    }

protected:
    bool createProxyFactory(const PluginInfo &pluginInfo, QObject *parent) override
    {
        auto *proxy = new Proxy(pluginInfo, parent);
        if (!proxy->isValid()) {
            m_errors << PluginLoadError(pluginInfo.path(), qApp->translate("GammaRay::PluginManager", "Failed to load plugin: %1").arg(proxy->errorString()));
            std::cerr << "invalid plugin " << qPrintable(pluginInfo.path()) << std::endl;
            delete proxy;
        } else {
            m_plugins.push_back(proxy);
            return true;
        }
        return false;
    }

private:
    QVector<IFace *> m_plugins;
};
}

#endif // GAMMARAY_PLUGINMANAGER_H
