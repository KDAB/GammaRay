/*
  statemodeldelegate.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "statemodeldelegate.h"

using namespace GammaRay;

StateModelDelegate::StateModelDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void StateModelDelegate::initStyleOption(QStyleOptionViewItem *option,
                                         const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    // turn check marks into bold text, to save space and avoid the impression you can interact with this
    option->features = option->features & ~QStyleOptionViewItem::HasCheckIndicator;
    const auto active = index.sibling(index.row(), 0).data(Qt::CheckStateRole).toInt() == Qt::Checked;
    option->font.setBold(active);
}
