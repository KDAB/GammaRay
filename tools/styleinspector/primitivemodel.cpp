/*
  primitivemodel.cpp

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

#include "primitivemodel.h"
#include "styleoption.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qstyleoption.h>

using namespace GammaRay;

struct primitive_element_t {
    const char *name;
    QStyle::PrimitiveElement primitive;
    QStyleOption* (*styleOptionFactory)();
};

#define MAKE_PE( primitive ) { #primitive , QStyle:: primitive, &StyleOption::makeStyleOption }
#define MAKE_PE_X( primitive, factory ) { #primitive, QStyle:: primitive, &StyleOption:: factory }

static primitive_element_t primititveElements[] =  {
  MAKE_PE(PE_Q3CheckListController),
  MAKE_PE(PE_Q3CheckListExclusiveIndicator),
  MAKE_PE(PE_Q3CheckListIndicator),
  MAKE_PE(PE_Q3DockWindowSeparator),
  MAKE_PE(PE_Q3Separator),
  MAKE_PE_X(PE_Frame, makeFrameStyleOption),
  MAKE_PE(PE_FrameDefaultButton),
  MAKE_PE_X(PE_FrameDockWidget, makeFrameStyleOption),
  MAKE_PE(PE_FrameFocusRect),
  MAKE_PE_X(PE_FrameGroupBox, makeFrameStyleOption),
  MAKE_PE_X(PE_FrameLineEdit, makeFrameStyleOption),
  MAKE_PE_X(PE_FrameMenu, makeFrameStyleOption),
  MAKE_PE(PE_FrameStatusBarItem),
  MAKE_PE(PE_FrameTabWidget),
  MAKE_PE_X(PE_FrameWindow, makeFrameStyleOption),
  MAKE_PE(PE_FrameButtonBevel),
  MAKE_PE(PE_FrameButtonTool),
  MAKE_PE(PE_FrameTabBarBase),
  MAKE_PE_X(PE_PanelButtonCommand, makeButtonStyleOption),
  MAKE_PE(PE_PanelButtonBevel),
  MAKE_PE(PE_PanelButtonTool),
  MAKE_PE(PE_PanelMenuBar),
  MAKE_PE(PE_PanelToolBar),
  MAKE_PE_X(PE_PanelLineEdit, makeFrameStyleOption),
  MAKE_PE(PE_IndicatorArrowDown),
  MAKE_PE(PE_IndicatorArrowLeft),
  MAKE_PE(PE_IndicatorArrowRight),
  MAKE_PE(PE_IndicatorArrowUp),
  MAKE_PE(PE_IndicatorBranch),
  MAKE_PE(PE_IndicatorButtonDropDown),
  MAKE_PE(PE_IndicatorViewItemCheck),
  MAKE_PE_X(PE_IndicatorCheckBox, makeButtonStyleOption),
  MAKE_PE(PE_IndicatorDockWidgetResizeHandle),
  MAKE_PE_X(PE_IndicatorHeaderArrow, makeHeaderStyleOption),
  MAKE_PE(PE_IndicatorMenuCheckMark),
  MAKE_PE(PE_IndicatorProgressChunk),
  MAKE_PE_X(PE_IndicatorRadioButton, makeButtonStyleOption),
  MAKE_PE(PE_IndicatorSpinDown),
  MAKE_PE(PE_IndicatorSpinMinus),
  MAKE_PE(PE_IndicatorSpinPlus),
  MAKE_PE(PE_IndicatorSpinUp),
  MAKE_PE(PE_IndicatorToolBarHandle),
  MAKE_PE(PE_IndicatorToolBarSeparator),
  MAKE_PE(PE_PanelTipLabel),
  MAKE_PE(PE_IndicatorTabTear),
  MAKE_PE(PE_PanelScrollAreaCorner),
  MAKE_PE(PE_Widget),
  MAKE_PE(PE_IndicatorColumnViewArrow),
  MAKE_PE(PE_IndicatorItemViewItemDrop),
  MAKE_PE_X(PE_PanelItemViewItem, makeItemViewStyleOption),
  MAKE_PE(PE_PanelItemViewRow),
  MAKE_PE(PE_PanelStatusBar),
  MAKE_PE(PE_IndicatorTabClose),
  MAKE_PE(PE_PanelMenu)
};

PrimitiveModel::PrimitiveModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void PrimitiveModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant PrimitiveModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole && index.column() == 0)
    return primititveElements[index.row()].name;

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

    QScopedPointer<QStyleOption> opt((primititveElements[index.row()].styleOptionFactory)());
    opt->rect = pixmap.rect();
    opt->palette = m_style->standardPalette();
    opt->state = StyleOption::prettyState(index.column() - 1);
    m_style->drawPrimitive(primititveElements[index.row()].primitive, opt.data(), &painter);
    return pixmap;
  }

  if (role == Qt::SizeHintRole && index.column() > 0) {
    return QSize(68, 68);
  }

  return QVariant();
}

int PrimitiveModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1 + StyleOption::stateCount();
}

int PrimitiveModel::rowCount(const QModelIndex& parent) const
{
  if (!m_style || parent.isValid())
    return 0;
  return sizeof(primititveElements) / sizeof(primititveElements[0]) ;
}

QVariant PrimitiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0)
      return tr("Primitive Element");
    return StyleOption::stateDisplayName(section - 1);
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "primitivemodel.moc"
