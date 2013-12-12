/*
  fontbrowser.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
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

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSER_H
#define GAMMARAY_FONTBROWSER_FONTBROWSER_H

#include <core/toolfactory.h>

#include "fontbrowserserver.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QApplication>
#else
#include <QGuiApplication>
#endif

namespace GammaRay {

class FontBrowserFactory : public QObject
#ifndef Q_MOC_RUN // Qt4 moc fails on the ifdef'ed multi-inheritance and generates invalid code
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
, public StandardToolFactory<QApplication, FontBrowserServer>
#else
, public StandardToolFactory<QGuiApplication, FontBrowserServer>
#endif
#endif
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.FontBrowser")

  public:
    explicit FontBrowserFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Fonts");
    }
};

}

#endif // GAMMARAY_FONTBROWSER_H
