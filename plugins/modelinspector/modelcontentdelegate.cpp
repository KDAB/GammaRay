/*
  modelcontentdelegate.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "modelcontentdelegate.h"
#include "modelcontentproxymodel.h"

#include <QApplication>
#include <QBrush>
#include <QStyle>

using namespace GammaRay;

ModelContentDelegate::ModelContentDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

ModelContentDelegate::~ModelContentDelegate() = default;

void ModelContentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &origOption, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
    auto option = origOption;
    initStyleOption(&option, index);
    if (index.data(ModelContentProxyModel::DisabledRole).toBool())
        option.state = option.state & ~QStyle::State_Enabled;

    if (index.data(ModelContentProxyModel::SelectedRole).toBool()) {
        // for non-selected cells
        option.backgroundBrush = option.palette.highlight();
        option.backgroundBrush.setStyle(Qt::BDiagPattern);

        // TODO also render selection for currently selected cells
    }

    if (index.data(ModelContentProxyModel::IsDisplayStringEmptyRole).toBool()) {
        option.palette.setColor(QPalette::Text, option.palette.color(QPalette::Disabled, QPalette::Text));
        option.text = tr("<unnamed: row %1, column %2>").arg(index.row()).arg(index.column());
    }

    QStyle *style = QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &option, painter, nullptr);
}
