/*
  textdocumentformatmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
