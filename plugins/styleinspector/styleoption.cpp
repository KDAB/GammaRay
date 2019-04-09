/*
  styleoption.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "styleoption.h"

#include <QStyleOption>

using namespace GammaRay;

// The following commit in qtbase.git deprecated versioned style class in 5.7.0
//   95a6dc1 - QStyleOption*V<N>: mark as Q_DECL_DEPRECATED
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
using QStyleOptionFrameCV = QStyleOptionFrame;
using QStyleOptionToolBoxCV = QStyleOptionToolBox;
using QStyleOptionTabCV = QStyleOptionTab;
using QStyleOptionTabBarBaseCV = QStyleOptionTabBarBase;
using QStyleOptionTabWidgetFrameCV = QStyleOptionTabWidgetFrame;
using QStyleOptionViewItemCV = QStyleOptionViewItem;
using QStyleOptionProgressBarCV = QStyleOptionProgressBar;
#else
typedef QStyleOptionFrameV3 QStyleOptionFrameCV;
typedef QStyleOptionToolBoxV2 QStyleOptionToolBoxCV;
typedef QStyleOptionTabBarBaseV2 QStyleOptionTabBarBaseCV;
typedef QStyleOptionTabV3 QStyleOptionTabCV;
typedef QStyleOptionTabWidgetFrameV2 QStyleOptionTabWidgetFrameCV;
typedef QStyleOptionViewItemV4 QStyleOptionViewItemCV;
typedef QStyleOptionProgressBarV2 QStyleOptionProgressBarCV;
#endif

struct style_state_t {
    const char *name;
    QStyle::State state;
};

#define MAKE_STATE(state) { #state, QStyle:: state }

static const style_state_t styleStates[] = {
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

int StyleOption::stateCount()
{
    return sizeof(styleStates) / sizeof(style_state_t);
}

QString StyleOption::stateDisplayName(int index)
{
    return QString::fromLatin1(styleStates[index].name).mid(6); // remove the State_ prefix
}

QStyle::State StyleOption::prettyState(int index)
{
    QStyle::State s = styleStates[index].state;
    if (s == QStyle::State_None)
        return s;
    return s | QStyle::State_Enabled; // enable by default, else we usually see disabled stuff only
}

QStyleOption *StyleOption::makeStyleOption()
{
    return new QStyleOption;
}

QStyleOption *StyleOption::makeStyleOptionComplex()
{
    return new QStyleOptionComplex;
}

QStyleOption *StyleOption::makeButtonStyleOption()
{
    auto *opt = new QStyleOptionButton;
    opt->features = QStyleOptionButton::None;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeComboBoxStyleOption()
{
    auto *opt = new QStyleOptionComboBox;
    opt->frame = true;
    opt->currentText = QStringLiteral("Current Text");
    return opt;
}

QStyleOption *StyleOption::makeFrameStyleOption()
{
    auto opt = new QStyleOptionFrameCV;
    opt->lineWidth = 1;
    opt->midLineWidth = 0;
    opt->frameShape = QFrame::StyledPanel;
    return opt;
}

QStyleOption *StyleOption::makeGroupBoxStyleOption()
{
    auto *opt = new QStyleOptionGroupBox;
    opt->lineWidth = 1;
    opt->midLineWidth = 0;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeHeaderStyleOption()
{
    auto *opt = new QStyleOptionHeader;
    opt->orientation = Qt::Horizontal;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeItemViewStyleOption()
{
    auto opt = new QStyleOptionViewItemCV;
    opt->text = QStringLiteral("Text");
    opt->features = QStyleOptionViewItemCV::HasDisplay;
    return opt;
}

QStyleOption *StyleOption::makeMenuStyleOption()
{
    auto *opt = new QStyleOptionMenuItem;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeProgressBarStyleOption()
{
    auto opt = new QStyleOptionProgressBarCV;
    opt->minimum = 0;
    opt->maximum = 100;
    opt->progress = 42;
    return opt;
}

QStyleOption *StyleOption::makeSliderStyleOption()
{
    auto *opt = new QStyleOptionSlider;
    opt->minimum = 0;
    opt->maximum = 100;
    opt->sliderValue = 42;
    opt->tickInterval = 5;
    return opt;
}

QStyleOption *StyleOption::makeSpinBoxStyleOption()
{
    auto *opt = new QStyleOptionSpinBox;
    opt->frame = true;
    return opt;
}

QStyleOption *StyleOption::makeTabStyleOption()
{
    auto opt = new QStyleOptionTabCV;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeTabBarBaseStyleOption()
{
    return new QStyleOptionTabBarBaseCV;
}

QStyleOption *StyleOption::makeTabWidgetFrameStyleOption()
{
    auto opt = new QStyleOptionTabWidgetFrameCV;
    opt->lineWidth = 1;
    return opt;
}

QStyleOption *StyleOption::makeTitleBarStyleOption()
{
    auto *opt = new QStyleOptionTitleBar;
    opt->text = QStringLiteral("Title");
    opt->titleBarFlags = Qt::WindowMinMaxButtonsHint
                         |Qt::WindowTitleHint
                         |Qt::WindowSystemMenuHint
                         |Qt::WindowCloseButtonHint;
    return opt;
}

QStyleOption *StyleOption::makeToolBoxStyleOption()
{
    auto opt = new QStyleOptionToolBoxCV;
    opt->text = QStringLiteral("Label");
    return opt;
}

QStyleOption *StyleOption::makeToolButtonStyleOption()
{
    auto *opt = new QStyleOptionToolButton;
    opt->text = QStringLiteral("Label");
    opt->toolButtonStyle = Qt::ToolButtonFollowStyle;
    return opt;
}
