/*
  toolmanager.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_TOOLMANAGER_H
#define GAMMARAY_TOOLMANAGER_H

#include "proxytoolfactory.h"

#include <common/toolmanagerinterface.h>
#include <common/pluginmanager.h>

namespace GammaRay {
class ToolFactory;
class ProxyToolFactory;

typedef PluginManager<ToolFactory, ProxyToolFactory> ToolPluginManager;

/** @brief Server-sided tool manager. Provides information to the client
 *  about which tools exists and especially which tools to enable. Also
 *  calls the ToolPluginManager to load tool plugins.
 */
class ToolManager : public ToolManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolManagerInterface)
public:
    explicit ToolManager(QObject *parent = nullptr);

    /** returns the tools that are best suited to show information about \p object. */
    QVector<QString> toolsForObject(QObject *object) const;
    /** returns the tools that are best suited to show information about \p object. */
    QVector<QString> toolsForObject(const void *object, const QString &typeName) const;

    bool hasTool(const QString &id) const;

    ToolPluginManager *toolPluginManager() const;

    /** Check if we have to activate tools for this type */
    void objectAdded(QObject *obj);

    void selectTool(const QString &toolId);

private:
    /**
     * Check if we have to activate tools for this type
     *
     * NOTE: must be called from the GUI thread
     */
    void objectAdded(const QMetaObject *mo);

public slots:
    void selectObject(const GammaRay::ObjectId &id, const QString &toolId) override;
    void requestToolsForObject(const GammaRay::ObjectId &id) override;
    void requestAvailableTools() override;

private:
    void addToolFactory(ToolFactory *tool);
    ToolData toolInfoForFactory(ToolFactory *factory) const;

    QVector<ToolFactory *> m_tools;
    QSet<ToolFactory *> m_disabledTools;
    QSet<const QMetaObject *> m_knownMetaObjects;
    QScopedPointer<ToolPluginManager> m_toolPluginManager;
};
}

#endif // GAMMARAY_TOOLMANAGER_H
