/*
  widgetinspector_export_actions.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

/**
 * @file widgetinspector_export_actions.cpp
 * dlopen hack to avoid dependencies on QtSvg and QtDesigner in the main probe.
 */

#include "uiextractor.h"

#include <QPainter>
#include <QSvgGenerator>
#include <QWidget>
#include <QFile>

extern "C" {

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

Q_DECL_EXPORT void gammaray_save_widget_to_ui(QWidget *widget, const QString &fileName)
{
  QFile file(fileName);
  if (file.open(QFile::WriteOnly)) {
    GammaRay::UiExtractor formBuilder;
    formBuilder.save(&file, widget);
  }
}

}
