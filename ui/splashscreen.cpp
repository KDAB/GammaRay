/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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
