/*
  editabletypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "editabletypesmodel.h"
#include "propertyeditor/propertyeditorfactory.h"

using namespace GammaRay;

EditableTypesModel::EditableTypesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_types = PropertyEditorFactory::supportedTypes();
}

EditableTypesModel::~EditableTypesModel() = default;

int EditableTypesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_types.size();
}

QVariant EditableTypesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int type = m_types.at(index.row());
    if (role == Qt::DisplayRole)
        return QMetaType::typeName(type);
    else if (role == Qt::UserRole)
        return type;

    return QVariant();
}
