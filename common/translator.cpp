/*
  translator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "translator.h"
#include "paths.h"

#include <compat/qasconst.h>

#include <QCoreApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QTranslator>

using namespace GammaRay;

static QString rootTranslationsPath() {
    return Paths::rootPath() + QLatin1Char('/') + GAMMARAY_TRANSLATION_INSTALL_DIR;
}

static QString qtTranslationsPath() {
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
}

void Translator::loadTranslations(const QString &catalog, const QString &path, const QString &overrideLanguage)
{
    const QDir dir(path);
    const QLocale locale;
    QStringList names = locale.uiLanguages();
    if (!overrideLanguage.isEmpty())
        names.prepend(overrideLanguage);

    for (const QString &name : qAsConst(names)) {
        const QLocale uiLocale(name);
        auto translator = new QTranslator(QCoreApplication::instance());
        if (translator->load(uiLocale, catalog, QStringLiteral("_"), path)) {
            QCoreApplication::instance()->installTranslator(translator);
            return;
        } else {
            delete translator;

            foreach (const QString &name, uiLocale.uiLanguages()) {
                const QString fileName = QString("%1_%2.qm").arg(catalog, name);
                const QString filePath = dir.filePath(fileName);
                if (QFile::exists(filePath)) {
                    return;
                }
            }
        }
    }

    if (locale.language() != QLocale::C && locale.language() != QLocale::English)
        qDebug() << "did not find a translation for" << catalog << "in" << path
                 << "for language" << locale.name();
}

void Translator::loadGammaRayTranslations(const QString &overrideLanguage)
{
    loadTranslations(QStringLiteral("gammaray"), rootTranslationsPath(), overrideLanguage);
}

void Translator::loadStandAloneTranslations(const QString &overrideLanguage)
{
    loadGammaRayTranslations(overrideLanguage);
    loadTranslations(QStringLiteral("qt"), qtTranslationsPath(), overrideLanguage);
}


