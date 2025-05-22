/*
  editabletypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        return QMetaType(type).name();
    else if (role == Qt::UserRole)
        return type;

    return QVariant();
}
