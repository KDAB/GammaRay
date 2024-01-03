/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/probe.h>
#include <ui/mainwindow.h>

extern "C" {
void Q_DECL_EXPORT gammaray_create_inprocess_mainwindow()
{
    auto *window = new GammaRay::MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    GammaRay::Probe::instance()->setWindow(window);
    GammaRay::Probe::instance()->setParent(window);
    window->show();

    QMetaObject::invokeMethod(window, "selectTool", Q_ARG(QString, QStringLiteral("GammaRay::ObjectInspector")));
}
}
