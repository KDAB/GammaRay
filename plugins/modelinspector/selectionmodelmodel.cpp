/*
  selectionmodelmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "selectionmodelmodel.h"

using namespace GammaRay;

SelectionModelModel::SelectionModelModel(QObject *parent) :
    ObjectTypeFilterProxyModel<QItemSelectionModel>(parent),
    m_model(Q_NULLPTR)
{
}

SelectionModelModel::~SelectionModelModel()
{
}

void SelectionModelModel::setModel(QAbstractItemModel* model)
{
    beginResetModel();
    m_model = model;
    endResetModel();
}

bool SelectionModelModel::filterAcceptsObject(QObject* object) const
{
    if (!m_model)
        return false;
    if (!ObjectTypeFilterProxyModel<QItemSelectionModel>::filterAcceptsObject(object))
        return false;

    auto selectionModel = qobject_cast<QItemSelectionModel*>(object);
    Q_ASSERT(selectionModel);
    return selectionModel->model() == m_model;
}
