/*
  sidepane.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "sidepane.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace GammaRay;

class Delegate : public QStyledItemDelegate
{
  public:
    explicit Delegate(QObject *parent = 0)
      : QStyledItemDelegate(parent)
    {
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
      static const int heightMargin = 10;

      QSize size = QStyledItemDelegate::sizeHint(option, index);
      size.setHeight(size.height() + heightMargin);
      return size;
    }
};

SidePane::SidePane(QWidget *parent)
  : QListView(parent)
{
  viewport()->setAutoFillBackground(false);
  setAttribute(Qt::WA_MacShowFocusRect, false);

  setItemDelegate(new Delegate(this));
}

SidePane::~SidePane()
{
}

QSize SidePane::sizeHint() const
{
  static const int widthMargin = 10;

  if (!model()) {
    return QSize(0, 0);
  }

  const int width = sizeHintForColumn(0) + widthMargin;
  const int height = QListView::sizeHint().height();

  return QSize(width, height);
}

void SidePane::setModel(QAbstractItemModel *model)
{
  if (model) {
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(updateSizeHint()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateSizeHint()));
  }
  QAbstractItemView::setModel(model);
}

void SidePane::resizeEvent(QResizeEvent *e)
{
  updateSizeHint();

  QListView::resizeEvent(e);
}

void SidePane::updateSizeHint()
{
  setMinimumWidth(sizeHint().width());
}

