/*
  fontbrowserclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERCLIENT_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERCLIENT_H

#include "fontbrowserinterface.h"

namespace GammaRay {
class FontBrowserClient : public FontBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::FontBrowserInterface)
public:
    explicit FontBrowserClient(QObject *parent = nullptr);

public slots:
    void setPointSize(int size) override;
    void toggleBoldFont(bool bold) override;
    void toggleItalicFont(bool italic) override;
    void toggleUnderlineFont(bool underline) override;
    void updateText(const QString &text) override;
    void setColors(const QColor &foreground, const QColor &background) override;
};
}

#endif // GAMMARAY_FONTBROWSERCLIENT_H
