/*
  aboutdata.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "aboutdata.h"
#include <config-gammaray-version.h>

#include <QDebug>
#include <QFile>
#include <QTextDocument>

using namespace GammaRay;

QStringList AboutData::authors()
{
    QFile f(":/gammaray/authors");
    if (f.open(QFile::ReadOnly)) {
      return QString::fromUtf8(f.readAll()).split('\n', QString::SkipEmptyParts);
    } else {
      Q_ASSERT_X(0, "AboutData::authors()", "cannot open the authors resource file");
      qWarning() << "Failed to open the authors resource file";
      return QStringList(QObject::tr("Unable to read the Authors list"));
    }
}

QStringList AboutData::authorsAsHtml()
{
    const auto plainAuthors = authors();
    QStringList a;
    a.reserve(plainAuthors.size());
    foreach (const QString &author, plainAuthors) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        a.push_back(Qt::escape(author));
#else
        a.push_back(author.toHtmlEscaped());
#endif
    }
    return a;
}

QString AboutData::aboutTitle()
{
    return QObject::trUtf8("<b>GammaRay %1</b>").arg(GAMMARAY_VERSION_STRING);
}

QString AboutData::aboutBody()
{
    return QObject::trUtf8(
        "<p>The Qt application inspection and manipulation tool."
        "Learn more at <a href=\"http://www.kdab.com/gammaray\">http://www.kdab.com/gammaray/</a>.</p>"
        "<p>Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, "
        "a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a></p>"
        "<p><u>Authors:</u><br>%1<br></p>"
        "<p>StackWalker code Copyright (c) 2005-2009, Jochen Kalmbach, All rights reserved</p>")
        .arg(authorsAsHtml().join("<br>"));
}

QString AboutData::aboutText()
{
    return aboutTitle() + aboutBody();
}
