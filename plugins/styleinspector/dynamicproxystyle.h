/*
  dynamicproxystyle.h

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

#ifndef GAMMARAY_STYLEINSPECTOR_DYNAMICPROXYSTYLE_H
#define GAMMARAY_STYLEINSPECTOR_DYNAMICPROXYSTYLE_H

#include <QHash>
#include <QProxyStyle>
#include <QPointer>

namespace GammaRay {
/**
 * A proxy style that allows runtime-editing of various parameters.
 */
class DynamicProxyStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit DynamicProxyStyle(QStyle *baseStyle);

    static DynamicProxyStyle *instance();
    static bool exists();
    static void insertProxyStyle();

    void setPixelMetric(PixelMetric metric, int value);
    void setStyleHint(StyleHint hint, int value);

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;
    int styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

private:
    QHash<QStyle::PixelMetric, int> m_pixelMetrics;
    QHash<QStyle::StyleHint, int> m_styleHints;
    static QPointer<DynamicProxyStyle> s_instance;
};
}

#endif // GAMMARAY_DYNAMICPROXYSTYLE_H
