/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "contextmenuextension.h"
#include "clienttoolmodel.h"
#include "uiintegration.h"

#include <common/objectbroker.h>
#include <common/probecontrollerinterface.h>

#include <QMenu>

using namespace GammaRay;

ContextMenuExtension::ContextMenuExtension(ObjectId id)
  : m_id(id)
{
}

void ContextMenuExtension::setGoToLocation(const SourceLocation &location)
{
  m_goToLoc = location;
}

void ContextMenuExtension::setShowSourceLocation(const SourceLocation &location)
{
  m_sourceLoc = location;
}

void ContextMenuExtension::setGoToCreationLocation(const SourceLocation& location)
{
  m_creationLoc = location;
}

void ContextMenuExtension::setGoToDeclarationLocation(const SourceLocation& location)
{
  m_declarationLoc = location;
}

void ContextMenuExtension::populateMenu(QMenu *menu)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  if (m_goToLoc.isValid() && UiIntegration::instance()) {
    auto action = menu->addAction(tr("Go to: %1").arg(m_goToLoc.displayString()));
    connect(action, &QAction::triggered, [this]() {
      UiIntegration::requestNavigateToCode(m_goToLoc.url(), m_goToLoc.line(), m_goToLoc.column());
    });
  }
  if (m_sourceLoc.isValid() && UiIntegration::instance()) {
    auto action = menu->addAction(tr("Show source: %1").arg(m_sourceLoc.displayString()));
    connect(action, &QAction::triggered, [this]() {
      UiIntegration::requestNavigateToCode(m_sourceLoc.url(), m_sourceLoc.line(), m_sourceLoc.column());
    });
  }
  if (m_creationLoc.isValid() && UiIntegration::instance()) {
    auto action = menu->addAction(tr("Go to creation: %1").arg(m_creationLoc.displayString()));
    connect(action, &QAction::triggered, [this]() {
      UiIntegration::requestNavigateToCode(m_creationLoc.url(), m_creationLoc.line(), m_creationLoc.column());
    });
  }
  if (m_declarationLoc.isValid() && UiIntegration::instance()) {
    auto action = menu->addAction(tr("Go to declaration: %1").arg(m_declarationLoc.displayString()));
    connect(action, &QAction::triggered, [this]() {
      UiIntegration::requestNavigateToCode(m_declarationLoc.url(), m_declarationLoc.line(), m_declarationLoc.column());
    });
  }

  if (m_id.isNull())
    return;

  auto probeController = ObjectBroker::object<ProbeControllerInterface*>();
  probeController->requestSupportedTools(m_id);

  // delay adding actions until we know the supported tools
  connect(probeController, &ProbeControllerInterface::supportedToolsResponse,
          menu, [menu](ObjectId id, const ToolInfos &toolInfos) {
    foreach (const auto &toolInfo, toolInfos) {
      auto action = menu->addAction(QObject::tr("Show in \"%1\" tool").arg(toolInfo.name));
      QObject::connect(action, &QAction::triggered, [id, toolInfo]() {
        auto probeController = ObjectBroker::object<ProbeControllerInterface*>();
        probeController->selectObject(id, toolInfo.id);
      });
    }
  });
#else
  Q_UNUSED(menu);
#endif
}
