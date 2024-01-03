/*
  proxytooluifactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
