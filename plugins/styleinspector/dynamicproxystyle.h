/*
  dynamicproxystyle.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEINSPECTOR_DYNAMICPROXYSTYLE_H
#define GAMMARAY_STYLEINSPECTOR_DYNAMICPROXYSTYLE_H

#include <QHash>
#include <QProxyStyle>

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

    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;

  private:
    QHash<QStyle::PixelMetric, int> m_pixelMetrics;
    static QWeakPointer<DynamicProxyStyle> s_instance;
};

}

#endif // GAMMARAY_DYNAMICPROXYSTYLE_H
