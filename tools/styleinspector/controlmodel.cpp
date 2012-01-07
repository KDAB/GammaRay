/*
  controlmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "controlmodel.h"
#include "styleoption.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

using namespace GammaRay;

struct control_element_t {
    const char *name;
    QStyle::ControlElement control;
    QStyleOption* (*styleOptionFactory)();
};

#define MAKE_CE( control ) { #control , QStyle:: control, &StyleOption::makeStyleOption }
#define MAKE_CE_X( control, factory ) { #control, QStyle:: control, &StyleOption:: factory }

static control_element_t controlElements[] =  {
  MAKE_CE_X(CE_PushButton, makeButtonStyleOption),
  MAKE_CE_X(CE_PushButtonBevel, makeButtonStyleOption),
  MAKE_CE_X(CE_PushButtonLabel, makeButtonStyleOption),
  MAKE_CE_X(CE_CheckBox, makeButtonStyleOption),
  MAKE_CE_X(CE_CheckBoxLabel, makeButtonStyleOption),
  MAKE_CE_X(CE_RadioButton, makeButtonStyleOption),
  MAKE_CE_X(CE_RadioButtonLabel, makeButtonStyleOption),
  MAKE_CE_X(CE_TabBarTab, makeTabStyleOption),
  MAKE_CE_X(CE_TabBarTabShape, makeTabStyleOption),
  MAKE_CE_X(CE_TabBarTabLabel, makeTabStyleOption),
  MAKE_CE_X(CE_ProgressBar, makeProgressBarStyleOption),
  MAKE_CE_X(CE_ProgressBarGroove, makeProgressBarStyleOption),
  MAKE_CE_X(CE_ProgressBarContents, makeProgressBarStyleOption),
  MAKE_CE_X(CE_ProgressBarLabel, makeProgressBarStyleOption),
  MAKE_CE(CE_MenuItem),
  MAKE_CE(CE_MenuScroller),
  MAKE_CE(CE_MenuVMargin),
  MAKE_CE(CE_MenuHMargin),
  MAKE_CE(CE_MenuTearoff),
  MAKE_CE(CE_MenuEmptyArea),
  MAKE_CE(CE_MenuBarItem),
  MAKE_CE(CE_MenuBarEmptyArea),
  MAKE_CE(CE_ToolButtonLabel),
  MAKE_CE(CE_Header),
  MAKE_CE(CE_HeaderSection),
  MAKE_CE(CE_HeaderLabel),
  MAKE_CE(CE_Q3DockWindowEmptyArea),
  MAKE_CE(CE_ToolBoxTab),
  MAKE_CE(CE_SizeGrip),
  MAKE_CE(CE_Splitter),
  MAKE_CE(CE_RubberBand),
  MAKE_CE(CE_DockWidgetTitle),
  MAKE_CE_X(CE_ScrollBarAddLine, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarSubLine, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarAddPage, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarSubPage, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarSlider, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarFirst, makeSliderStyleOption),
  MAKE_CE_X(CE_ScrollBarLast, makeSliderStyleOption),
  MAKE_CE(CE_FocusFrame),
  MAKE_CE(CE_ComboBoxLabel),
  MAKE_CE(CE_ToolBar),
  MAKE_CE_X(CE_ToolBoxTabShape, makeToolBoxStyleOption),
  MAKE_CE_X(CE_ToolBoxTabLabel, makeToolBoxStyleOption),
  MAKE_CE(CE_HeaderEmptyArea),
  MAKE_CE(CE_ColumnViewGrip),
  MAKE_CE(CE_ItemViewItem),
  MAKE_CE(CE_ShapedFrame)
};


ControlModel::ControlModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void ControlModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant ControlModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole && index.column() == 0)
    return controlElements[index.row()].name;

  if (role == Qt::DecorationRole && index.column() > 0) {
    QPixmap bgPattern(16,16);
    {
      bgPattern.fill(Qt::lightGray);
      QPainter painter(&bgPattern);
      painter.fillRect(8, 0, 8, 8, Qt::gray);
      painter.fillRect(0, 8, 8, 8, Qt::gray);
    }

    QPixmap pixmap(64,64);
    QPainter painter(&pixmap);
    QBrush bgBrush;
    bgBrush.setTexture(bgPattern);
    painter.fillRect(pixmap.rect(), bgBrush);

    QScopedPointer<QStyleOption> opt(controlElements[index.row()].styleOptionFactory());
    opt->rect = pixmap.rect();
    opt->palette = m_style->standardPalette();
    opt->state = StyleOption::prettyState(index.column() - 1);
    m_style->drawControl(controlElements[index.row()].control, opt.data(), &painter);
    return pixmap;
  }

  if (role == Qt::SizeHintRole && index.column() > 0) {
    return QSize(68, 68);
  }

  return QVariant();
}

int ControlModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1 + StyleOption::stateCount();
}

int ControlModel::rowCount(const QModelIndex& parent) const
{
  if (!m_style || parent.isValid())
    return 0;
  return sizeof(controlElements) / sizeof(controlElements[0]);
}

QVariant ControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0)
      return tr("Control Element");
    return StyleOption::stateDisplayName(section - 1);
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "controlmodel.moc"
