/*
  dynamicproxystyle.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "dynamicproxystyle.h"

#include <QApplication>

using namespace GammaRay;

QPointer<DynamicProxyStyle> DynamicProxyStyle::s_instance;

DynamicProxyStyle::DynamicProxyStyle(QStyle *baseStyle)
    : QProxyStyle(baseStyle)
{
    s_instance = QPointer<DynamicProxyStyle>(this);
}

DynamicProxyStyle *DynamicProxyStyle::instance()
{
    if (!s_instance)
        insertProxyStyle();
    return s_instance.data();
}

bool DynamicProxyStyle::exists()
{
    return s_instance;
}

void DynamicProxyStyle::insertProxyStyle()
{
    // TODO: if the current style is a CSS proxy, add us underneath
    // to avoid Qt adding yet another CSS proxy on top
    qApp->setStyle(new DynamicProxyStyle(qApp->style()));
}

void DynamicProxyStyle::setPixelMetric(QStyle::PixelMetric metric, int value)
{
    m_pixelMetrics.insert(metric, value);
}

void DynamicProxyStyle::setStyleHint(QStyle::StyleHint hint, int value)
{
    m_styleHints.insert(hint, value);
}

int DynamicProxyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option,
                                   const QWidget *widget) const
{
    auto it = m_pixelMetrics.find(metric);
    if (it != m_pixelMetrics.end())
        return it.value();
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int DynamicProxyStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    const auto it = m_styleHints.find(hint);
    if (it != m_styleHints.end())
        return it.value();
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
