/*
  proxytoolfactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "proxytoolfactory.h"

using namespace GammaRay;
using namespace std;

ProxyToolFactory::ProxyToolFactory(const PluginInfo &pluginInfo, QObject *parent)
  : ProxyFactory<ToolFactory>(pluginInfo, parent)
{
  const QStringList typesList = pluginInfo.supportedTypes();
  QVector<QByteArray> typesVector;
  typesVector.reserve(typesList.count());
  for (auto it = typesList.constBegin(), end = typesList.constEnd(); it != end; ++it) {
    typesVector << (*it).toLatin1();
  }
  setSupportedTypes(typesVector);
}

bool ProxyToolFactory::isValid() const
{
  return
    pluginInfo().isValid() &&
    !name().isEmpty() &&
    !supportedTypes().isEmpty();
}

QString ProxyToolFactory::name() const
{
  return pluginInfo().name();
}

void ProxyToolFactory::init(ProbeInterface *probe)
{
  loadPlugin();
  ToolFactory *fac = factory();
  if (!fac) {
    return;
  }
  Q_ASSERT(fac);
  fac->init(probe);
}

bool ProxyToolFactory::isHidden() const
{
  return pluginInfo().isHidden();
}

QVector<QByteArray> ProxyToolFactory::selectableTypes() const
{
    return pluginInfo().selectableTypes();
}
