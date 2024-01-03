/*
  proxyfactorybase.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "proxyfactorybase.h"

#include <QPluginLoader>

#include <iostream>

using namespace GammaRay;

ProxyFactoryBase::ProxyFactoryBase(const PluginInfo &pluginInfo, QObject *parent)
    : QObject(parent)
    , m_factory(nullptr)
    , m_pluginInfo(pluginInfo)
{
}

ProxyFactoryBase::~ProxyFactoryBase() = default;

PluginInfo ProxyFactoryBase::pluginInfo() const
{
    return m_pluginInfo;
}

QString ProxyFactoryBase::errorString() const
{
    return m_errorString;
}

void ProxyFactoryBase::loadPlugin()
{
    if (m_factory)
        return;

    if (!pluginInfo().isStatic()) {
        QPluginLoader loader(pluginInfo().path(), this);
        m_factory = loader.instance();
        if (!m_factory) {
            m_errorString = loader.errorString();
            std::cerr << "error loading plugin " << qPrintable(pluginInfo().path())
                      << ": " << qPrintable(loader.errorString()) << std::endl;
        }
    } else {
        m_factory = pluginInfo().staticInstance();
    }

    if (m_factory)
        m_factory->setParent(this);
}
