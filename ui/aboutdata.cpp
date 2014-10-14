#include <config-gammaray-version.h>
#include "aboutdata.h"

#include <QFile>
#include <QTextDocument>

using namespace GammaRay;

QStringList AboutData::authors()
{
    QFile f(":/gammaray/authors");
    f.open(QFile::ReadOnly);
    Q_ASSERT(f.isOpen());
    return QString::fromUtf8(f.readAll()).split("\n", QString::SkipEmptyParts);
}

QStringList AboutData::authorsAsHtml()
{
    QStringList a;
    foreach (const QString &author, authors()) {
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
        "<p>Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, "
        "a KDAB Group company, <a href=\"mailto:info@kdab.com\">info@kdab.com</a></p>"
        "<p><u>Authors:</u><br>%1<br></p>"
        "<p>StackWalker code Copyright (c) 2005-2009, Jochen Kalmbach, All rights reserved</p>")
        .arg(authorsAsHtml().join("<br>"));
}

QString AboutData::aboutText()
{
    return aboutTitle() + aboutBody();
}
