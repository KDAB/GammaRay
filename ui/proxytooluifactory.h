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
    Q_OBJECT
public:
    /**
     * @param pluginInfo Plugin spec file
     */
    explicit ProxyToolUiFactory(const PluginInfo &pluginInfo, QObject *parent = nullptr);

    QString name() const override;

    /** Returns @c true if the plugin seems valid from all the information we have so far. */
    bool isValid() const;

    bool remotingSupported() const override;
    QWidget *createWidget(QWidget *parentWidget) override;
    void initUi() override;
};
}

#endif // GAMMARAY_PROXYTOOLUIFACTORY_H
