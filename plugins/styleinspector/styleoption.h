/*
  styleoption.h

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

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEOPTION_H
#define GAMMARAY_STYLEINSPECTOR_STYLEOPTION_H

#include <QStyle>

QT_BEGIN_NAMESPACE
class QStyleOption;
class QString;
QT_END_NAMESPACE

namespace GammaRay {
/**
 * Various helper methods for dealing with QStyleOption.
 */
namespace StyleOption {
int stateCount();
QString stateDisplayName(int index);
QStyle::State prettyState(int index);

QStyleOption *makeStyleOption();
QStyleOption *makeStyleOptionComplex();

QStyleOption *makeButtonStyleOption();
QStyleOption *makeComboBoxStyleOption();
QStyleOption *makeFrameStyleOption();
QStyleOption *makeGroupBoxStyleOption();
QStyleOption *makeHeaderStyleOption();
QStyleOption *makeItemViewStyleOption();
QStyleOption *makeMenuStyleOption();
QStyleOption *makeProgressBarStyleOption();
QStyleOption *makeSliderStyleOption();
QStyleOption *makeSpinBoxStyleOption();
QStyleOption *makeTabStyleOption();
QStyleOption *makeTabBarBaseStyleOption();
QStyleOption *makeTabWidgetFrameStyleOption();
QStyleOption *makeTitleBarStyleOption();
QStyleOption *makeToolBoxStyleOption();
QStyleOption *makeToolButtonStyleOption();
}
}

#endif // GAMMARAY_STYLEOPTION_H
