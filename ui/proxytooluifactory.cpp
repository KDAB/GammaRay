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

#include "proxytooluifactory.h"

#include <QLabel>

using namespace GammaRay;

ProxyToolUiFactory::ProxyToolUiFactory(const QString &path, QObject *parent)
  : ProxyFactory<ToolUiFactory>(path, parent)
  , m_remotingSupported(false)
{
  m_remotingSupported = value(QLatin1String("X-GammaRay-Remote"), true).toBool();
}

bool ProxyToolUiFactory::isValid() const
{
  return
    !id().isEmpty() &&
    !m_pluginPath.isEmpty();
}

bool ProxyToolUiFactory::remotingSupported() const
{
  return m_remotingSupported;
}

QWidget *ProxyToolUiFactory::createWidget(QWidget *parentWidget)
{
  loadPlugin();
  ToolUiFactory *fac = factory();
  if (!fac) {
    return new QLabel(tr("Plugin '%1' could not be loaded.").arg(m_pluginPath), parentWidget);
  }
  Q_ASSERT(fac);
  return fac->createWidget(parentWidget);
}
