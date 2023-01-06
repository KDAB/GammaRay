/*
  promolabel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    setPixmap(UIResources::tintedPixmap(image, palette().windowText().color()));
}
