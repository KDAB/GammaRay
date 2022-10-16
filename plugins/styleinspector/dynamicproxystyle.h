/*
  dynamicproxystyle.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
