/*
  textdocumentformatmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textdocumentformatmodel.h"
#include "core/varianthandler.h"

#include <QMetaEnum>
#include <QMetaObject>

using namespace GammaRay;

static QMetaEnum propertyEnum()
{
    const int index = QTextFormat::staticMetaObject.indexOfEnumerator("Property");
    Q_ASSERT(index >= 0);
    return QTextFormat::staticMetaObject.enumerator(index);
}

TextDocumentFormatModel::TextDocumentFormatModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void TextDocumentFormatModel::setFormat(const QTextFormat &format)
{
    beginResetModel();
    m_format = format;
    endResetModel();
}

int TextDocumentFormatModel::rowCount(const QModelIndex &parent) const
{
    if (!m_format.isValid() || parent.isValid())
        return 0;
    return propertyEnum().keyCount();
}

int TextDocumentFormatModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TextDocumentFormatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const int enumValue = propertyEnum().value(index.row());
        switch (index.column()) {
        case 0:
            return QString::fromLatin1(propertyEnum().key(index.row()));
        case 1:
            return VariantHandler::displayString(m_format.property(enumValue));
        case 2:
            return QString::fromLatin1(m_format.property(enumValue).typeName());
        }
    } else if (role == Qt::DecorationRole && index.column() == 1) {
        const int enumValue = propertyEnum().value(index.row());
        return VariantHandler::decoration(m_format.property(enumValue));
    }

    return QVariant();
}

QVariant TextDocumentFormatModel::headerData(int section, Qt::Orientation orientation,
                                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Property");
        case 1:
            return tr("Value");
        case 2:
            return tr("Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
