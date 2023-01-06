/*
  wk2application.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQuickView view;
    view.setSource(QUrl(QStringLiteral("qrc:/wk2application.qml")));
    view.show();

    return app.exec();
}
