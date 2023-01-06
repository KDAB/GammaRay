/*
  aboutdata.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        return QString::fromUtf8(f.readAll()).split('\n', QString::SkipEmptyParts);
#else
        return QString::fromUtf8(f.readAll()).split('\n', Qt::SkipEmptyParts);
#endif
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
    return AboutDataContext::tr(
        "<p>The Qt application inspection and manipulation tool."
        "Learn more at <a href=\"https://www.kdab.com/gammaray\">https://www.kdab.com/gammaray/</a>.</p>"
        "<p>Copyright (C) 2010-2023 Klarälvdalens Datakonsult AB, "
        "a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a></p>"
        "<p>StackWalker code Copyright (c) 2005-2019, Jochen Kalmbach, All rights reserved<br>"
        "lz4 fast LZ compression code Copyright (C) 2011-2020, Yann Collet, All rights reserved<br>"
        "backward-cpp code Copyright 2013-2017 Google Inc. All rights reserved.</p>");
}

QString AboutData::aboutAuthors()
{
    return AboutDataContext::tr(
               "<p><u>Authors:</u><br>%1</p>")
        .arg(authorsAsHtml().join(QStringLiteral("<br>")));
}

QString AboutData::aboutFooter()
{
    return AboutDataContext::tr(
        "<p>GammaRay and the GammaRay logo are registered trademarks of Klarälvdalens Datakonsult AB "
        "in the European Union, the United States and/or other countries.  Other product and "
        "company names and logos may be trademarks or registered trademarks of their respective companies.</p>"
        "<br>"
        "Social: <a href=\"https://twitter.com/KDABQt\">Twitter</a>, "
        "<a href=\"https://www.facebook.com/kdabqt\">Facebook</a>, "
        "<a href=\"https://www.youtube.com/c/KDABtv\">YouTube</a>, "
        "<a href=\"https://www.kdab.com/category/blogs/\">Blogs</a>");
}
