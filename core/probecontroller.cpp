/*
  probecontroller.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "probecontroller.h"

#include "toolmodel.h"
#include "probe.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMutexLocker>

using namespace GammaRay;

ProbeController::ProbeController(QObject* parent)
  : ProbeControllerInterface(parent)
{
}

void ProbeController::selectObject(ObjectId id, const QString &toolId)
{
  switch (id.type()) {
  case ObjectId::Invalid:
    return;
  case ObjectId::QObjectType: {
    QMutexLocker lock(Probe::objectLock());
    if (!Probe::instance()->isValidObject(id.asQObject()))
      return;

    Probe::instance()->selectObject(id.asQObject(), toolId);
    break;
  }
  case ObjectId::VoidStarType:
    Probe::instance()->selectObject(id.asVoidStar(), id.typeName());
    break;
  }
}

void ProbeController::requestSupportedTools(ObjectId id)
{
  QModelIndexList indexes;
  switch (id.type()) {
  case ObjectId::Invalid:
    return;
  case ObjectId::QObjectType: {
    QMutexLocker lock(Probe::objectLock());
    if (!Probe::instance()->isValidObject(id.asQObject()))
      return;

    indexes = Probe::instance()->toolModel()->toolsForObject(id.asQObject());
    break;
  }
  case ObjectId::VoidStarType:
    const auto asVoidStar = reinterpret_cast<void *>(id.id());
    indexes = Probe::instance()->toolModel()->toolsForObject(asVoidStar, id.typeName());
    break;
  }

  ToolInfos toolInfos;
  toolInfos.reserve(indexes.size());
  foreach (const auto &index, indexes) {
    ToolInfo info;
    info.id = index.data(ToolModelRole::ToolId).toString();
    info.name =  index.data(Qt::DisplayRole).toString();
    toolInfos.push_back(info);
  }
  emit supportedToolsResponse(id, toolInfos);
}

void ProbeController::detachProbe()
{
  Probe::instance()->deleteLater();
}

void ProbeController::quitHost()
{
  QCoreApplication::instance()->quit();
}

