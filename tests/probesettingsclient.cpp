/*
  probesettingsclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/probesettings.h>

#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QTimer>

using namespace GammaRay;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    ProbeSettings::receiveSettings();

    QUrl addr = QUrl::fromUserInput(ProbeSettings::value("TestValue").toString());
    ProbeSettings::sendServerAddress(addr);

    QTimer::singleShot(1000, &app, &QCoreApplication::quit);
    return app.exec();
}
