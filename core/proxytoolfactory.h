/*
  proxytoolfactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROXYTOOLFACTORY_H
#define GAMMARAY_PROXYTOOLFACTORY_H

#include <common/proxyfactorybase.h>
#include "toolfactory.h"

namespace GammaRay {
/**
 * A wrapper around a plugin ToolFactory that only loads the actual plugin
 * once initialized.
 * Until then, meta-data is provided based on a plugin spec file.
 *
 * TODO: Improve error reporting
 */
class ProxyToolFactory : public ProxyFactory<ToolFactory>
{
    Q_OBJECT
public:
    /**
     * @param pluginInfo Plugin spec file
     */
    explicit ProxyToolFactory(const PluginInfo &pluginInfo, QObject *parent = nullptr);

    /** Returns @c true if the plugin seems valid from all the information we have so far. */
    bool isValid() const;

    bool isHidden() const override;
    QVector<QByteArray> selectableTypes() const override;

    void init(Probe *probe) override;
};
}

#endif // GAMMARAY_PROXYTOOLFACTORY_H
