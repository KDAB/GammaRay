/*
  modelevent.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelevent.h"

#include <QAbstractItemModel>
#include <QCoreApplication>

using namespace GammaRay;

ModelEvent::ModelEvent(bool modelUsed)
    : QEvent(eventType())
    , m_used(modelUsed)
{
}

ModelEvent::~ModelEvent() = default;

bool ModelEvent::used() const
{
    return m_used;
}

QEvent::Type ModelEvent::eventType()
{
    static int id = -1;
    if (id < 0)
        id = registerEventType();
    return static_cast<QEvent::Type>(id);
}

void Model::used(const QAbstractItemModel *model)
{
    Q_ASSERT(model);
    ModelEvent ev(true);
    QCoreApplication::sendEvent(const_cast<QAbstractItemModel *>(model), &ev); // const_cast hack is needed since QItemSelectionModel gives us const*...
}

void Model::unused(QAbstractItemModel *model)
{
    Q_ASSERT(model);
    ModelEvent ev(false);
    QCoreApplication::sendEvent(model, &ev);
}
