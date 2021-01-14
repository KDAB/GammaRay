/*
  controlmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "controlmodel.h"
#include "styleoption.h"
#include "styleinspectorinterface.h"
#include <core/util.h>

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

using namespace GammaRay;

struct control_element_t {
    const char *name;
    QStyle::ControlElement control;
    QStyleOption *(*styleOptionFactory)();
};

#define MAKE_CE(control) { #control, QStyle:: control, &StyleOption::makeStyleOption }
#define MAKE_CE_X(control, factory) { #control, QStyle:: control, &StyleOption:: factory }

static const control_element_t controlElements[] = {
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
    MAKE_CE_X(CE_MenuItem, makeMenuStyleOption),
    MAKE_CE(CE_MenuScroller),
    MAKE_CE(CE_MenuVMargin),
    MAKE_CE(CE_MenuHMargin),
    MAKE_CE(CE_MenuTearoff),
    MAKE_CE(CE_MenuEmptyArea),
    MAKE_CE_X(CE_MenuBarItem, makeMenuStyleOption),
    MAKE_CE(CE_MenuBarEmptyArea),
    MAKE_CE_X(CE_ToolButtonLabel, makeToolButtonStyleOption),
    MAKE_CE_X(CE_Header, makeHeaderStyleOption),
    MAKE_CE_X(CE_HeaderSection, makeHeaderStyleOption),
    MAKE_CE_X(CE_HeaderLabel, makeHeaderStyleOption),
    MAKE_CE_X(CE_ToolBoxTab, makeToolBoxStyleOption),
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
    MAKE_CE_X(CE_ComboBoxLabel, makeComboBoxStyleOption),
    MAKE_CE(CE_ToolBar),
    MAKE_CE_X(CE_ToolBoxTabShape, makeToolBoxStyleOption),
    MAKE_CE_X(CE_ToolBoxTabLabel, makeToolBoxStyleOption),
    MAKE_CE_X(CE_HeaderEmptyArea, makeHeaderStyleOption),
    MAKE_CE(CE_ColumnViewGrip),
    MAKE_CE_X(CE_ItemViewItem, makeItemViewStyleOption),
    MAKE_CE_X(CE_ShapedFrame, makeFrameStyleOption)
};

ControlModel::ControlModel(QObject *parent)
    : AbstractStyleElementStateTable(parent)
{
}

QVariant ControlModel::doData(int row, int column, int role) const
{
    if (role == Qt::DecorationRole) {
        QPixmap pixmap(m_interface->cellSizeHint());
        QPainter painter(&pixmap);
        Util::drawTransparencyPattern(&painter, pixmap.rect());
        painter.scale(m_interface->cellZoom(), m_interface->cellZoom());

        QScopedPointer<QStyleOption> opt(controlElements[row].styleOptionFactory());
        fillStyleOption(opt.data(), column);
        m_style->drawControl(controlElements[row].control, opt.data(), &painter);
        return pixmap;
    }

    return AbstractStyleElementStateTable::doData(row, column, role);
}

int ControlModel::doRowCount() const
{
    return sizeof(controlElements) / sizeof(controlElements[0]);
}

QVariant ControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return controlElements[section].name;
    return AbstractStyleElementStateTable::headerData(section, orientation, role);
}
