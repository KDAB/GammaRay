/*
  qtivipropertyclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qtivipropertyclientmodel.h"
#include "qtivipropertymodel.h"

#include <QApplication>
#include <QFont>

using namespace GammaRay;

QtIviPropertyClientModel::QtIviPropertyClientModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

QtIviPropertyClientModel::~QtIviPropertyClientModel() = default;

QVariant QtIviPropertyClientModel::data(const QModelIndex &index, int role) const
{
    // QIviProperty's appears in italic to distinguish from plain qt properties
    if (role == Qt::FontRole) {
        const bool isIviProperty = QSortFilterProxyModel::data(index, QtIviPropertyModel::IsIviProperty).toBool();
        QFont font = QApplication::font("QAbstractItemView");
        font.setItalic(isIviProperty);
        return QVariant::fromValue(font);
    }

    return QSortFilterProxyModel::data(index, role);
}
