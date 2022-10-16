/*
  widgetpaintanalyzerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETPAINTANALYZEREXTENSION_H
#define GAMMARAY_WIDGETPAINTANALYZEREXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace GammaRay {
class PaintAnalyzer;
class PropertyController;

class WidgetPaintAnalyzerExtension : public PropertyControllerExtension
{
public:
    explicit WidgetPaintAnalyzerExtension(PropertyController *controller);
    ~WidgetPaintAnalyzerExtension();

    bool setQObject(QObject *object) override;

private:
    void analyze();

    PaintAnalyzer *m_paintAnalyzer;
    QWidget *m_widget;
};
}

#endif // GAMMARAY_WIDGETPAINTANALYZEREXTENSION_H
