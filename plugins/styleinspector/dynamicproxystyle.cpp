/*
  dynamicproxystyle.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "dynamicproxystyle.h"

#include <QApplication>

using namespace GammaRay;

QWeakPointer<DynamicProxyStyle> DynamicProxyStyle::s_instance;

DynamicProxyStyle::DynamicProxyStyle(QStyle *baseStyle)
  : QProxyStyle(baseStyle)
{
  s_instance = QWeakPointer<DynamicProxyStyle>(this);
}

DynamicProxyStyle *DynamicProxyStyle::instance()
{
  if (!s_instance) {
    insertProxyStyle();
  }
  return s_instance.data();
}

bool DynamicProxyStyle::exists()
{
  return s_instance;
}

void DynamicProxyStyle::insertProxyStyle()
{
  // TODO: if the current style is a CSS proxy, add us underneath
  //       to avoid Qt adding yet another CSS proxy on top
  qApp->setStyle(new DynamicProxyStyle(qApp->style()));
}

void DynamicProxyStyle::setPixelMetric(QStyle::PixelMetric metric, int value)
{
  m_pixelMetrics.insert(metric, value);
}

int DynamicProxyStyle::pixelMetric(QStyle::PixelMetric metric,
                                   const QStyleOption *option,
                                   const QWidget *widget) const
{
  QHash<QStyle::PixelMetric, int>::const_iterator it = m_pixelMetrics.find(metric);
  if (it != m_pixelMetrics.end()) {
    return it.value();
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}
