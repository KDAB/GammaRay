/*
  mainwindow.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "promolabel.h"

#include <ui/uiresources.h>

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QMouseEvent>

using namespace GammaRay;

PromoLabel::PromoLabel(QWidget *parent, Qt::WindowFlags f)
    : ThemedImageLabel(parent, f)
{
    setCursor(QCursor(Qt::PointingHandCursor));
    setToolTip(tr("Visit KDAB Website"));
    setThemeFileName(QStringLiteral("kdab-products.png"));
}

bool PromoLabel::event(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange)
        updatePixmap();
    return ThemedImageLabel::event(e);
}

void PromoLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::NoModifier) {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.kdab.com")));
        ev->accept();
        return;
    }

    ThemedImageLabel::mouseReleaseEvent(ev);
}

void PromoLabel::updatePixmap()
{
    // load image and adapt it to user's foreground color
    const QImage image = UIResources::themedImage(themeFileName(), this);
    setPixmap(UIResources::tintedPixmap(image, palette().foreground().color()));
}
