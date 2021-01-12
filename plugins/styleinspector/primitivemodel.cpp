/*
  primitivemodel.cpp

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

#include "primitivemodel.h"
#include "styleoption.h"
#include "styleinspectorinterface.h"
#include <core/util.h>

#include <QPainter>
#include <QPixmap>
#include <QStyleOption>

using namespace GammaRay;

struct primitive_element_t {
    const char *name;
    QStyle::PrimitiveElement primitive;
    QStyleOption *(*styleOptionFactory)();
};

#define MAKE_PE(primitive) { #primitive, QStyle:: primitive, &StyleOption::makeStyleOption }
#define MAKE_PE_X(primitive, factory) { #primitive, QStyle:: primitive, &StyleOption:: factory }

static const primitive_element_t primititveElements[] = {
    MAKE_PE_X(PE_Frame, makeFrameStyleOption),
    MAKE_PE(PE_FrameDefaultButton),
    MAKE_PE_X(PE_FrameDockWidget, makeFrameStyleOption),
    MAKE_PE(PE_FrameFocusRect),
    MAKE_PE_X(PE_FrameGroupBox, makeFrameStyleOption),
    MAKE_PE_X(PE_FrameLineEdit, makeFrameStyleOption),
    MAKE_PE_X(PE_FrameMenu, makeFrameStyleOption),
    MAKE_PE(PE_FrameStatusBarItem),
    MAKE_PE_X(PE_FrameTabWidget, makeTabWidgetFrameStyleOption),
    MAKE_PE_X(PE_FrameWindow, makeFrameStyleOption),
    MAKE_PE(PE_FrameButtonBevel),
    MAKE_PE(PE_FrameButtonTool),
    MAKE_PE_X(PE_FrameTabBarBase, makeTabBarBaseStyleOption),
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MAKE_PE(PE_IndicatorViewItemCheck),
#endif
    MAKE_PE_X(PE_IndicatorCheckBox, makeButtonStyleOption),
    MAKE_PE(PE_IndicatorDockWidgetResizeHandle),
    MAKE_PE_X(PE_IndicatorHeaderArrow, makeHeaderStyleOption),
    MAKE_PE(PE_IndicatorMenuCheckMark),
    MAKE_PE(PE_IndicatorProgressChunk),
    MAKE_PE_X(PE_IndicatorRadioButton, makeButtonStyleOption),
    MAKE_PE_X(PE_IndicatorSpinDown, makeSpinBoxStyleOption),
    MAKE_PE_X(PE_IndicatorSpinMinus, makeSpinBoxStyleOption),
    MAKE_PE_X(PE_IndicatorSpinPlus, makeSpinBoxStyleOption),
    MAKE_PE_X(PE_IndicatorSpinUp, makeSpinBoxStyleOption),
    MAKE_PE(PE_IndicatorToolBarHandle),
    MAKE_PE(PE_IndicatorToolBarSeparator),
    MAKE_PE(PE_PanelTipLabel),
    MAKE_PE_X(PE_IndicatorTabTear, makeTabStyleOption),
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

PrimitiveModel::PrimitiveModel(QObject *parent)
    : AbstractStyleElementStateTable(parent)
{
}

QVariant PrimitiveModel::doData(int row, int column, int role) const
{
    if (role == Qt::DecorationRole) {
        QPixmap pixmap(m_interface->cellSizeHint());
        QPainter painter(&pixmap);
        Util::drawTransparencyPattern(&painter, pixmap.rect());
        painter.scale(m_interface->cellZoom(), m_interface->cellZoom());

        QScopedPointer<QStyleOption> opt((primititveElements[row].styleOptionFactory)());
        fillStyleOption(opt.data(), column);
        m_style->drawPrimitive(primititveElements[row].primitive, opt.data(), &painter);
        return pixmap;
    }

    return AbstractStyleElementStateTable::doData(row, column, role);
}

int PrimitiveModel::doRowCount() const
{
    return sizeof(primititveElements) / sizeof(primititveElements[0]);
}

QVariant PrimitiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return primititveElements[section].name;
    return AbstractStyleElementStateTable::headerData(section, orientation, role);
}
