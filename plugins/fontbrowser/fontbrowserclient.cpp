/*
  fontbrowserclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "fontbrowserclient.h"

#include <common/endpoint.h>

#include <QColor>

using namespace GammaRay;

FontBrowserClient::FontBrowserClient(QObject *parent)
    : FontBrowserInterface(parent)
{
}

#define WRAP_REMOTE(func, type)                                                         \
    void FontBrowserClient::func(type arg)                                              \
    {                                                                                   \
        Endpoint::instance()->invokeObject(objectName(), #func, QVariantList() << arg); \
    }

WRAP_REMOTE(setPointSize, int)
WRAP_REMOTE(toggleBoldFont, bool)
WRAP_REMOTE(toggleItalicFont, bool)
WRAP_REMOTE(toggleUnderlineFont, bool)
WRAP_REMOTE(updateText, const QString &)

void FontBrowserClient::setColors(const QColor &foreground, const QColor &background)
{
    Endpoint::instance()->invokeObject(objectName(), "setColors",
                                       QVariantList() << foreground << background);
}
