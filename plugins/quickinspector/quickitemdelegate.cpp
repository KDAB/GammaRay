/*
  quickitemdelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "quickitemdelegate.h"
#include "quickitemmodelroles.h"
#include <QPainter>
#include <QIcon>
#include <QVariant>
#include <QTreeView>
#include <QApplication>

using namespace GammaRay;

QuickItemDelegate::QuickItemDelegate(QTreeView *view)
  : QStyledItemDelegate(view),
    m_view(view)
{
}

void QuickItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
  int flags = index.data(QuickItemModelRole::ItemFlags).value<int>();

  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  // Disable foreground painting so we can do ourself
  opt.text = "";
  opt.icon = QIcon();

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

  QRect drawRect = option.rect;

  QColor base = option.state & QStyle::State_Selected ?
                  option.palette.highlightedText().color() :
                  index.data(Qt::ForegroundRole).value<QColor>();

  if (m_colors.contains(index.sibling(index.row(), 0))) {
    QColor blend = m_colors.value(index.sibling(index.row(), 0));
    QColor blended =
      QColor::fromRgbF(base.redF()   * (1 - blend.alphaF()) + blend.redF()   * blend.alphaF(),
                       base.greenF() * (1 - blend.alphaF()) + blend.greenF() * blend.alphaF(),
                       base.blueF()  * (1 - blend.alphaF()) + blend.blueF()  * blend.alphaF());
    painter->setPen(blended);
  } else {
    painter->setPen(base);
  }

  if (index.column() == 0) {
    QVector<QPixmap> icons;
    icons << index.data(Qt::DecorationRole).value<QPixmap>();

    if ((flags & QuickItemModelRole::OutOfView) && (~flags & QuickItemModelRole::Invisible)) {
      icons << QIcon(":/gammaray/plugins/quickinspector/warning.png").pixmap(16, 16);
    }

    if (flags & QuickItemModelRole::HasActiveFocus) {
      icons << QIcon(":/gammaray/plugins/quickinspector/active-focus.png").pixmap(16, 16);
    }

    if (flags & QuickItemModelRole::HasFocus && ~flags & QuickItemModelRole::HasActiveFocus) {
      icons << QIcon(":/gammaray/plugins/quickinspector/focus.png").pixmap(16, 16);
    }

    for (int i = 0; i < icons.size(); i++) {
      painter->drawPixmap(drawRect.topLeft(), icons.at(i));
      drawRect.setTopLeft(drawRect.topLeft() + QPoint(20, 0));
    }
  }

  painter->drawText(drawRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
}

QSize QuickItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
  Q_UNUSED(option);

  QSize textSize =
    m_view->fontMetrics().size(Qt::TextSingleLine, index.data(Qt::DisplayRole).toString());

  QSize decorationSize;
  if (index.column() == 0) {
    int flags = index.data(QuickItemModelRole::ItemFlags).value<int>();

    int icons = 1;
    if ((flags & QuickItemModelRole::OutOfView) && (~flags & QuickItemModelRole::Invisible)) {
      icons++;
    }

    if (flags & (QuickItemModelRole::HasFocus | QuickItemModelRole::HasActiveFocus)) {
      icons++;
    }

    decorationSize = QSize(icons * 20, 16);
  }

  return QSize(textSize.width() + decorationSize.width() + 5,
               qMax(textSize.height(), decorationSize.height()));
}

void QuickItemDelegate::setTextColor(const QVariant &textColor)
{
  const QPersistentModelIndex index = sender()->property("index").value<QPersistentModelIndex>();
  if (!index.isValid()) {
    return;
  }

  m_colors[index] = textColor.value<QColor>();

  for (int i = 0; i < m_view->model()->columnCount(); i++) {
    m_view->update(index.sibling(index.row(), i));
  }
}
