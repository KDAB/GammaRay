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
