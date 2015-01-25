/*
  fontbrowserclient.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERCLIENT_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERCLIENT_H

#include "fontbrowserinterface.h"

namespace GammaRay {

class FontBrowserClient : public FontBrowserInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::FontBrowserInterface)
  public:
    explicit FontBrowserClient(QObject *parent = 0);

  public slots:
    virtual void setPointSize(int size);
    virtual void toggleBoldFont(bool bold);
    virtual void toggleItalicFont(bool italic);
    virtual void toggleUnderlineFont(bool underline);
    virtual void updateText(const QString &text);
    virtual void setColors(const QColor &foreground, const QColor &background);
};

}

#endif // GAMMARAY_FONTBROWSERCLIENT_H
