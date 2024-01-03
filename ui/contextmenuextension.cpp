/*
  contextmenuextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "contextmenuextension.h"
#include "clienttoolmanager.h"
#include "uiintegration.h"

#include <common/objectbroker.h>
#include <common/propertymodel.h>
#include <common/modelroles.h>
#include <common/favoriteobjectinterface.h>

#include <QCoreApplication>
#include <QMenu>
#include <QModelIndex>

using namespace GammaRay;

namespace {
QString sourceLocationLabel(ContextMenuExtension::Location location,
                            const SourceLocation &sourceLocation)
{
    switch (location) {
    case ContextMenuExtension::GoTo:
        return qApp->translate("GammaRay::ContextMenuExtension",
                               "Go to: %1")
            .arg(sourceLocation.displayString());
    case ContextMenuExtension::ShowSource:
        return qApp->translate("GammaRay::ContextMenuExtension",
                               "Show source: %1")
            .arg(sourceLocation.displayString());
    case ContextMenuExtension::Creation:
        return qApp->translate("GammaRay::ContextMenuExtension",
                               "Go to creation: %1")
            .arg(sourceLocation.displayString());
    case ContextMenuExtension::Declaration:
        return qApp->translate("GammaRay::ContextMenuExtension",
                               "Go to declaration: %1")
            .arg(sourceLocation.displayString());
    }
    Q_ASSERT(false);
    return QString();
}
}

ContextMenuExtension::ContextMenuExtension(const ObjectId &id)
    : m_id(id)
{
}

void ContextMenuExtension::setLocation(ContextMenuExtension::Location location,
                                       const SourceLocation &sourceLocation)
{
    m_locations.push_back(QPair<Location, SourceLocation>(location, sourceLocation));
}

void ContextMenuExtension::setCanFavoriteItems(bool v)
{
    canFavorite = v;
}

bool ContextMenuExtension::discoverSourceLocation(ContextMenuExtension::Location location,
                                                  const QUrl &url)
{
    if (!UiIntegration::instance())
        return false;

    if (url.isEmpty())
        return false;

    setLocation(location, SourceLocation(url));
    return true;
}

bool ContextMenuExtension::discoverPropertySourceLocation(ContextMenuExtension::Location location,
                                                          const QModelIndex &index)
{
    if (!UiIntegration::instance())
        return false;

    if (!index.isValid())
        return false;

    const bool isUrl = index.sibling(index.row(), PropertyModel::TypeColumn).data().toString() == QStringLiteral("QUrl");
    if (!isUrl)
        return false;

    return discoverSourceLocation(location, index.sibling(index.row(), PropertyModel::ValueColumn).data().toUrl());
}

void ContextMenuExtension::populateMenu(QMenu *menu)
{
    if (UiIntegration::instance()) {
        for (auto it = m_locations.constBegin(), end = m_locations.constEnd(); it != end; ++it) {
            if (it->second.isValid()) {
                auto action = menu->addAction(sourceLocationLabel(it->first, it->second));
                QObject::connect(action, &QAction::triggered, UiIntegration::instance(), [it]() {
                    UiIntegration::requestNavigateToCode(it->second.url(), it->second.line(),
                                                         it->second.column());
                });
            }
        }
    }

    if (m_id.isNull())
        return;

    Q_ASSERT(ClientToolManager::instance());
    ClientToolManager::instance()->requestToolsForObject(m_id);

    // delay adding actions until we know the supported tools
    QObject::connect(ClientToolManager::instance(), &ClientToolManager::toolsForObjectResponse,
                     menu, [menu](const ObjectId &id, const QVector<ToolInfo> &toolInfos) {
                         for (const auto &toolInfo : toolInfos) {
                             auto action = menu->addAction(qApp->translate("GammaRay::ContextMenuExtension",
                                                                           "Show in \"%1\" tool")
                                                               .arg(toolInfo.name()));
                             QObject::connect(action, &QAction::triggered, [id, toolInfo]() {
                                 ClientToolManager::instance()->selectObject(id, toolInfo);
                             });
                         }
                     });

    if (canFavorite) {
        menu->addAction(QStringLiteral("Favorite"), [this] {
            if (auto iface = ObjectBroker::object<FavoriteObjectInterface *>())
                iface->markObjectAsFavorite(m_id);
        });
    }
}
