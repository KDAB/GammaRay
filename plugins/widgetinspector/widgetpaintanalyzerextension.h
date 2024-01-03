/*
  widgetpaintanalyzerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
