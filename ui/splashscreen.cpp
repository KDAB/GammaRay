/*
  splashscreen.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "splashscreen.h"
#include "uiresources.h"

#include <QSplashScreen>
#include <QBitmap>
#include <QApplication>
#include <QScreen>

QSplashScreen *splash = nullptr;

namespace GammaRay {
void showSplashScreen()
{
    if (!splash) {
        splash = new QSplashScreen;
        QPixmap pixmap = UIResources::themedPixmap(QStringLiteral("splashscreen.png"), splash);
        splash->setPixmap(pixmap);
    }

    const QWidget *window = qApp->activeWindow();

    if (window && window != splash) {
        splash->ensurePolished();

        const QRect windowRect = splash->screen()->availableGeometry();
        QRect splashRect = QRect(QPoint(), splash->size());

        splashRect.moveCenter(windowRect.center());
        splash->move(splashRect.topLeft());
    }

    splash->show();
}

void hideSplashScreen()
{
    if (splash) {
        splash->hide();
        delete splash;
        splash = nullptr;
    }
}
}
