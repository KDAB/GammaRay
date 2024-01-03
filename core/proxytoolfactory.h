/*
  proxytoolfactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
