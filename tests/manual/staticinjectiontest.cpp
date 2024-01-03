/*
  staticinjectiontest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/staticprobe.h>

#include <QApplication>
#include <QLabel>

GAMMARAY_STATIC_INJECT

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QLabel label("Hello World");
    label.show();
    return app.exec();
}
