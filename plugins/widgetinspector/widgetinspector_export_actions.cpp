/*
  widgetinspector_export_actions.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

/**
 * @file widgetinspector_export_actions.cpp
 * dlopen hack to avoid dependencies on QtSvg and QtDesigner in the main probe.
 */

#include <config-gammaray.h>
#include "uiextractor.h"

#ifdef HAVE_QT_SVG
#include <QSvgGenerator>
#endif

#include <QPainter>
#include <QWidget>
#include <QFile>

extern "C" {
#ifdef HAVE_QT_SVG
Q_DECL_EXPORT void gammaray_save_widget_to_svg(QWidget *widget, const QString &fileName)
{
    QSvgGenerator svg;
    svg.setFileName(fileName);
    svg.setSize(widget->size());
    svg.setViewBox(QRect(QPoint(0, 0), widget->size()));
    QPainter painter(&svg);
    widget->render(&painter);
    painter.end();
}

#endif

#ifdef HAVE_QT_DESIGNER
Q_DECL_EXPORT void gammaray_save_widget_to_ui(QWidget *widget, const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        GammaRay::UiExtractor formBuilder;
        formBuilder.save(&file, widget);
    }
}

#endif
}
