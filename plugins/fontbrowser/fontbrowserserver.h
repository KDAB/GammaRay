/*
  fontbrowserserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERSERVER_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERSERVER_H

#include "fontbrowserinterface.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
class FontModel;
class Probe;

class FontBrowserServer : public FontBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::FontBrowserInterface)
public:
    explicit FontBrowserServer(Probe *probe, QObject *parent = nullptr);

private slots:
    void updateFonts();

    void setPointSize(int size) override;
    void toggleBoldFont(bool bold) override;
    void toggleItalicFont(bool italic) override;
    void toggleUnderlineFont(bool underline) override;
    void updateText(const QString &text) override;
    void setColors(const QColor &foreground, const QColor &background) override;

private:
    FontModel *m_selectedFontModel;
    QItemSelectionModel *m_fontSelectionModel;
};
}

#endif // FONTBROWSERSERVER_H
