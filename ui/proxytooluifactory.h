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

#ifndef GAMMARAY_PROXYTOOLUIFACTORY_H
#define GAMMARAY_PROXYTOOLUIFACTORY_H

#include <common/proxyfactorybase.h>
#include <ui/tooluifactory.h>

namespace GammaRay {

/**
 * A wrapper around a plugin ToolUiFactory that only loads the actual plugin once really needed.
 * Until then, meta-data is provided based on a plugin spec file.
 */
class ProxyToolUiFactory : public ProxyFactory<ToolUiFactory>
{
  public:
    /**
     * @param path Path to the plugin spec file
     */
    explicit ProxyToolUiFactory(const QString &path, QObject *parent = 0);

    /** Returns @c true if the plugin seems valid from all the information we have so far. */
    bool isValid() const;

    /*override*/ bool remotingSupported() const;
    /*override*/ QWidget *createWidget(QWidget *parentWidget);

private:
  bool m_remotingSupported;
};

}

#endif // GAMMARAY_PROXYTOOLUIFACTORY_H
