/*
  widgetinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H

#include "widgetinspectorserver.h"

#include <core/toolfactory.h>

#include <QWidget>

namespace GammaRay {
class WidgetInspectorFactory : public QObject,
                               public StandardToolFactory<QWidget, WidgetInspectorServer>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_widgetinspector.json")
public:
    explicit WidgetInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_WIDGETINSPECTOR_H
