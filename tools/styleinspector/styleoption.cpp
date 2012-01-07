/*
  styleoption.cpp

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

#include "styleoption.h"

#include <QStyleOption>

using namespace GammaRay;

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
  return s | QStyle::State_Enabled; // enable by default, otherwise we usually only see disabled stuff
}

QStyleOption* StyleOption::makeStyleOption()
{
  return new QStyleOption;
}

QStyleOption* StyleOption::makeFrameStyleOption()
{
  QStyleOptionFrameV3 *opt = new QStyleOptionFrameV3;
  opt->lineWidth = 1;
  opt->midLineWidth = 0;
  opt->frameShape = QFrame::StyledPanel;
  return opt;
}

QStyleOption* StyleOption::makeButtonStyleOption()
{
  QStyleOptionButton *opt = new QStyleOptionButton;
  opt->features = QStyleOptionButton::None;
  opt->text = QLatin1String("Label");
  return opt;
}

QStyleOption* StyleOption::makeItemViewStyleOption()
{
  QStyleOptionViewItemV4 *opt = new QStyleOptionViewItemV4;
  return opt;
}

QStyleOption* StyleOption::makeHeaderStyleOption()
{
  QStyleOptionHeader *opt = new QStyleOptionHeader;
  opt->orientation = Qt::Horizontal;
  opt->text = QLatin1String("Label");
  return opt;
}

QStyleOption* StyleOption::makeMenuStyleOption()
{
  QStyleOptionMenuItem *opt = new QStyleOptionMenuItem;
  opt->text = QLatin1String("Label");
  return opt;
}

QStyleOption* StyleOption::makeProgressBarStyleOption()
{
  QStyleOptionProgressBarV2 *opt = new QStyleOptionProgressBarV2;
  opt->minimum = 0;
  opt->maximum = 100;
  opt->progress = 42;
  return opt;
}

QStyleOption* StyleOption::makeSliderStyleOption()
{
  QStyleOptionSlider *opt = new QStyleOptionSlider;
  opt->minimum = 0;
  opt->maximum = 100;
  opt->sliderValue = 42;
  return opt;
}


QStyleOption* StyleOption::makeTabStyleOption()
{
  QStyleOptionTabV3* opt = new QStyleOptionTabV3;
  opt->text = QLatin1String("label");
  return opt;
}

QStyleOption* StyleOption::makeToolBoxStyleOption()
{
  QStyleOptionToolBoxV2* opt = new QStyleOptionToolBoxV2;
  opt->text = QLatin1String("Label");
  return opt;
}
