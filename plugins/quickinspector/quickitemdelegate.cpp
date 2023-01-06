/*
  quickitemdelegate.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "quickitemdelegate.h"
#include "quickitemmodelroles.h"

#include <ui/uiresources.h>

#include <QPainter>
#include <QIcon>
#include <QVariant>
#include <QAbstractItemView>
#include <QApplication>

using namespace GammaRay;

QuickItemDelegate::QuickItemDelegate(QAbstractItemView *view)
    : QStyledItemDelegate(view)
    , m_view(view)
{
}

void QuickItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    painter->save();
    int flags = index.data(QuickItemModelRole::ItemFlags).value<int>();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Disable foreground painting so we can do ourself
    opt.text.clear();
    opt.icon = QIcon();

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    QRect drawRect = option.rect;
    painter->setClipRect(option.rect);
    painter->setClipping(true); // avoid the icons leaking into the next column

    const auto foregroundData = index.data(Qt::ForegroundRole);
    const auto inactiveColor = foregroundData.isNull() ? option.palette.text().color() : foregroundData.value<QColor>();
    const auto base = option.state
            & QStyle::State_Selected
        ? option.palette.highlightedText().color()
        : inactiveColor;

    if (m_colors.contains(index.sibling(index.row(), 0))) {
        QColor blend = m_colors.value(index.sibling(index.row(), 0));
        QColor blended = QColor::fromRgbF(
            base.redF() * (1 - blend.alphaF()) + blend.redF() * blend.alphaF(),
            base.greenF() * (1 - blend.alphaF()) + blend.greenF() * blend.alphaF(),
            base.blueF() * (1 - blend.alphaF()) + blend.blueF() * blend.alphaF());
        painter->setPen(blended);
    } else {
        painter->setPen(base);
    }

    if (index.column() == 0) {
        auto deco = index.data(Qt::DecorationRole);
        QVector<QPixmap> icons;
        if (deco.canConvert<QPixmap>())
            icons.push_back(deco.value<QPixmap>());
        else if (deco.canConvert<QIcon>())
            icons.push_back(deco.value<QIcon>().pixmap(16, 16));

        if ((flags & QuickItemModelRole::PartiallyOutOfView) && (~flags & QuickItemModelRole::Invisible))
            icons << UIResources::themedIcon(QStringLiteral("warning.png")).pixmap(16, 16);

        if (flags & QuickItemModelRole::HasActiveFocus)
            icons << UIResources::themedIcon(QStringLiteral("active-focus.png")).pixmap(16, 16);

        if (flags & QuickItemModelRole::HasFocus && ~flags & QuickItemModelRole::HasActiveFocus)
            icons << UIResources::themedIcon(QStringLiteral("focus.png")).pixmap(16, 16);

        for (int i = 0; i < icons.size() && drawRect.left() < opt.rect.right(); i++) {
            painter->drawPixmap(drawRect.topLeft(), icons.at(i));
            drawRect.setTopLeft(drawRect.topLeft() + QPoint(20, 0));
        }
    }

    painter->drawText(drawRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
    painter->restore();
}

QSize QuickItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(option);

    // this gets us the cached value for empty cells
    const auto sh = index.data(Qt::SizeHintRole);
    if (sh.isValid())
        return sh.toSize();

    QSize textSize = m_view->fontMetrics().size(Qt::TextSingleLine, index.data(Qt::DisplayRole).toString());

    QSize decorationSize;
    if (index.column() == 0) {
        int flags = index.data(QuickItemModelRole::ItemFlags).value<int>();

        int icons = 1;
        if ((flags & QuickItemModelRole::OutOfView) && (~flags & QuickItemModelRole::Invisible))
            icons++;

        if (flags & (QuickItemModelRole::HasFocus | QuickItemModelRole::HasActiveFocus))
            icons++;

        decorationSize = QSize(icons * 20, 16);
    }

    return { textSize.width() + decorationSize.width() + 5,
             qMax(textSize.height(), decorationSize.height()) };
}

void QuickItemDelegate::setTextColor(const QVariant &textColor, const QPersistentModelIndex &index)
{
    if (!index.isValid())
        return;

    m_colors[index] = textColor.value<QColor>();

    for (int i = 0; i < m_view->model()->columnCount(); i++)
        m_view->update(index.sibling(index.row(), i));
}
