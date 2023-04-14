/*
  splashscreen.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "splashscreen.h"
#include "uiresources.h"

#include <QSplashScreen>
#include <QBitmap>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

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

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        const QRect windowRect = qApp->desktop()->availableGeometry(window);
#else
        const QRect windowRect = splash->screen()->availableGeometry();
#endif
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
