/*
  styleoption.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
