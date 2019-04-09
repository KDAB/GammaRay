/*
  aboutdata.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextDocument>

using namespace GammaRay;

namespace GammaRay {
class AboutDataContext
{
    Q_DECLARE_TR_FUNCTIONS(GammaRay::AboutDataContext)
};
}

QStringList AboutData::authors()
{
    QFile f(QStringLiteral(":/gammaray/authors"));
    if (f.open(QFile::ReadOnly)) {
        return QString::fromUtf8(f.readAll()).split('\n', QString::SkipEmptyParts);
    } else {
        Q_ASSERT_X(false, "AboutData::authors()", "cannot open the authors resource file");
        qWarning() << "Failed to open the authors resource file";
        return QStringList(AboutDataContext::tr("Unable to read the Authors list"));
    }
}

QStringList AboutData::authorsAsHtml()
{
    const auto plainAuthors = authors();
    QStringList a;
    a.reserve(plainAuthors.size());
    for (const QString &author : plainAuthors) {
        a.push_back(author.toHtmlEscaped());
    }
    return a;
}

QString AboutData::aboutTitle()
{
    return AboutDataContext::tr("<b>GammaRay %1</b>").arg(QStringLiteral(GAMMARAY_VERSION_STRING));
}

QString AboutData::aboutHeader()
{
    return AboutDataContext::trUtf8(
                "<p>The Qt application inspection and manipulation tool."
                "Learn more at <a href=\"https://www.kdab.com/gammaray\">https://www.kdab.com/gammaray/</a>.</p>"
                "<p>Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, "
                "a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a></p>"
                "<p>StackWalker code Copyright (c) 2005-2009, Jochen Kalmbach, All rights reserved<br>"
                "lz4 fast LZ compression code Copyright (C) 2011-2015, Yann Collet, All rights reserved<br>"
                "Backward-cpp code Copyright 2013 Google Inc. All rights reserved.</p>");
}

QString AboutData::aboutAuthors()
{
    return AboutDataContext::trUtf8(
                "<p><u>Authors:</u><br>%1</p>"
                ).arg(authorsAsHtml().join(QStringLiteral("<br>")));
}

QString AboutData::aboutFooter()
{
    return AboutDataContext::trUtf8(
                "<p>GammaRay and the GammaRay logo are registered trademarks of Klarälvdalens Datakonsult AB "
                "in the European Union, the United States and/or other countries.  Other product and "
                "company names and logos may be trademarks or registered trademarks of their respective companies.</p>"
                "<br>");
}
