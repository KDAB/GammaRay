/*
  modelcontentdelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelcontentdelegate.h"
#include "modelcontentproxymodel.h"

#include <QApplication>
#include <QBrush>
#include <QStyle>

using namespace GammaRay;

ModelContentDelegate::ModelContentDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
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
