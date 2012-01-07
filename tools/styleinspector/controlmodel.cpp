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

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

using namespace GammaRay;

struct control_element_t {
    const char *name;
    QStyle::ControlElement control;
};

#define MAKE_CE( control ) { #control , QStyle:: control }

static control_element_t controlElements[] =  {
  MAKE_CE(CE_PushButton),
  MAKE_CE(CE_PushButtonBevel),
//   MAKE_CE(CE_PushButtonLabel),
  MAKE_CE(CE_CheckBox),
  MAKE_CE(CE_CheckBoxLabel),
  MAKE_CE(CE_RadioButton),
  MAKE_CE(CE_RadioButtonLabel),
  MAKE_CE(CE_TabBarTab),
  MAKE_CE(CE_TabBarTabShape),
  MAKE_CE(CE_TabBarTabLabel),
  MAKE_CE(CE_ProgressBar),
  MAKE_CE(CE_ProgressBarGroove),
//   MAKE_CE(CE_ProgressBarContents),
  MAKE_CE(CE_ProgressBarLabel),
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
  MAKE_CE(CE_ScrollBarAddLine),
  MAKE_CE(CE_ScrollBarSubLine),
  MAKE_CE(CE_ScrollBarAddPage),
  MAKE_CE(CE_ScrollBarSubPage),
  MAKE_CE(CE_ScrollBarSlider),
  MAKE_CE(CE_ScrollBarFirst),
  MAKE_CE(CE_ScrollBarLast),
  MAKE_CE(CE_FocusFrame),
  MAKE_CE(CE_ComboBoxLabel),
  MAKE_CE(CE_ToolBar),
  MAKE_CE(CE_ToolBoxTabShape),
//   MAKE_CE(CE_ToolBoxTabLabel),
  MAKE_CE(CE_HeaderEmptyArea),
  MAKE_CE(CE_ColumnViewGrip),
  MAKE_CE(CE_ItemViewItem),
  MAKE_CE(CE_ShapedFrame)
};

// TODO refactor and share with other models
struct style_state_t {
  const char *name;
  QStyle::State state;
};

#define MAKE_STATE( state ) { #state, QStyle:: state }

static style_state_t styleStates[] = {
  MAKE_STATE(State_None),
  MAKE_STATE(State_Enabled),
  MAKE_STATE(State_Raised),
  MAKE_STATE(State_Sunken),
  MAKE_STATE(State_Off),
  MAKE_STATE(State_NoChange),
  MAKE_STATE(State_On),
  MAKE_STATE(State_DownArrow),
  MAKE_STATE(State_Horizontal),
  MAKE_STATE(State_HasFocus),
  MAKE_STATE(State_Top),
  MAKE_STATE(State_Bottom),
  MAKE_STATE(State_FocusAtBorder),
  MAKE_STATE(State_AutoRaise),
  MAKE_STATE(State_MouseOver),
  MAKE_STATE(State_UpArrow),
  MAKE_STATE(State_Selected),
  MAKE_STATE(State_Active),
  MAKE_STATE(State_Window),
  MAKE_STATE(State_Open),
  MAKE_STATE(State_Children),
  MAKE_STATE(State_Item),
  MAKE_STATE(State_Sibling),
  MAKE_STATE(State_Editing),
  MAKE_STATE(State_KeyboardFocusChange),
  MAKE_STATE(State_ReadOnly),
  MAKE_STATE(State_Small),
  MAKE_STATE(State_Mini)
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

    QScopedPointer<QStyleOption> opt( new QStyleOption );
    opt->rect = pixmap.rect();
    opt->palette = m_style->standardPalette();
    opt->state = styleStates[index.column()-1].state;
    if (opt->state != QStyle::State_None)
      opt->state |= QStyle::State_Enabled; // enable all other states as well, otherwise we'll only see disabled primitives
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
  return 1 + sizeof(styleStates) / sizeof(styleStates[0]);
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
    return QString::fromLatin1(styleStates[section-1].name).mid(6); // remove State_ prefix
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "controlmodel.moc"
