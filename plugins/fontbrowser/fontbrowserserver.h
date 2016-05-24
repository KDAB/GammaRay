/*
  fontbrowserserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
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

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERSERVER_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERSERVER_H

#include "fontbrowserinterface.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {

class ProbeInterface;
class FontModel;

class FontBrowserServer : public FontBrowserInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::FontBrowserInterface)
  public:
    explicit FontBrowserServer(ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void updateFonts();

    void setPointSize(int size) Q_DECL_OVERRIDE;
    void toggleBoldFont(bool bold) Q_DECL_OVERRIDE;
    void toggleItalicFont(bool italic) Q_DECL_OVERRIDE;
    void toggleUnderlineFont(bool underline) Q_DECL_OVERRIDE;
    void updateText(const QString &text) Q_DECL_OVERRIDE;
    void setColors(const QColor &foreground, const QColor &background) Q_DECL_OVERRIDE;

  private:
    FontModel *m_selectedFontModel;
    QItemSelectionModel *m_fontSelectionModel;
};

}

#endif // FONTBROWSERSERVER_H
