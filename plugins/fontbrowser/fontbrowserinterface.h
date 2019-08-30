/*
  fontbrowserinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERINTERFACE_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERINTERFACE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QColor;
QT_END_NAMESPACE

namespace GammaRay {
class FontBrowserInterface : public QObject
{
    Q_OBJECT
public:
    explicit FontBrowserInterface(QObject *parent);
    ~FontBrowserInterface() override;

    enum {
        FontRole = Qt::UserRole + 1,
        FontSearchRole = Qt::UserRole + 2,
        SortRole = Qt::UserRole + 3,
    };

public slots:
    virtual void updateText(const QString &text) = 0;
    virtual void toggleBoldFont(bool bold) = 0;
    virtual void toggleItalicFont(bool italic) = 0;
    virtual void toggleUnderlineFont(bool underline) = 0;
    virtual void setPointSize(int size) = 0;
    virtual void setColors(const QColor &foreground, const QColor &background) = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::FontBrowserInterface, "com.kdab.GammaRay.FontBrowser")
QT_END_NAMESPACE

#endif // FONTBROWSERINTERFACE_H
