/*
  ljsonupdate.cpp

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

#include "config-gammaray-version.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTranslator>

static int extractMessages(const QStringList &jsons, const QString &outFileName)
{
    QFile out(outFileName);
    if (!out.open(QFile::WriteOnly)) {
        qWarning() << "Can't open output file" << outFileName;
        return 1;
    }

    for (const auto &jsonFile : jsons) {
        QFile inFile(jsonFile);
        if (!inFile.open(QFile::ReadOnly)) {
            qWarning() << "Can't open input file" << jsonFile;
            return 1;
        }
        const auto doc = QJsonDocument::fromJson(inFile.readAll());
        const auto obj = doc.object();
        const auto str = obj.value("name").toString(); // TODO make the keys configurable too
        if (str.isEmpty())
            continue;
        out.write(R"(QT_TRANSLATE_NOOP("GammaRay::PluginMetaData", ")");
        out.write(str.toUtf8());
        out.write("\");\n");
    }

    return 0;
}

static int mergeMessages(const QStringList &jsons, const QString &qmFile, const QString &lang)
{
    QTranslator t;
    if (!t.load(qmFile)) {
        if (QFile::exists(qmFile)) {
            qDebug() << "Translation catalog empty, skipping.";
            return 0;
        } else {
            qWarning() << "Can't open translation catalog" << qmFile;
            return 1;
        }
    }

    for (const auto &jsonFile : jsons) {
        QFile inFile(jsonFile);
        if (!inFile.open(QFile::ReadOnly)) {
            qWarning() << "Can't open input file" << jsonFile;
            return 1;
        }

        const auto doc = QJsonDocument::fromJson(inFile.readAll());
        auto obj = doc.object();
        const auto sourceString = obj.value("name").toString(); // TODO make the keys configurable too
        if (sourceString.isEmpty())
            continue;

        const auto trString = t.translate("GammaRay::PluginMetaData", sourceString.toUtf8());
        if (trString.isEmpty() || trString == sourceString)
            continue;

        obj.insert(QLatin1String("name[") + lang + ']', trString);
        inFile.close();
        QFile outFile(jsonFile);
        if (!outFile.open(QFile::WriteOnly)) {
            qWarning() << "Can't write to JSON file" << jsonFile;
            return 1;
        }
        outFile.write(QJsonDocument(obj).toJson());
    }

    return 0;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ljsonupdate");
    QCoreApplication::setApplicationVersion(GAMMARAY_VERSION_STRING);

    QCommandLineParser parser;
    parser.setApplicationDescription("Translation update tool for JSON plug-in meta data.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption extractCommand("extract", "Extract messages from the given JSON files.");
    parser.addOption(extractCommand);
    QCommandLineOption outputFile("out", "Output of the extraction.", "output-file");
    parser.addOption(outputFile);

    QCommandLineOption mergeCommand("merge", "Merge messages in the given JSON files.");
    parser.addOption(mergeCommand);
    QCommandLineOption qmFile("qm", ".qm file with translated messages.", "qm-file");
    parser.addOption(qmFile);
    QCommandLineOption language("lang", "Language identifier for translated JSON keys.", "lang");
    parser.addOption(language);

    parser.addPositionalArgument("json", "JSON files to process.");

    parser.process(app);

    auto jsons = parser.positionalArguments();
    if (jsons.isEmpty()) {
        qWarning() << "No input JSON files specified.";
        return 1;
    }

    if (parser.isSet(extractCommand)) {
        std::sort(jsons.begin(), jsons.end()); // stabilize the output between different runs
        return extractMessages(jsons, parser.value(outputFile));
    } else if (parser.isSet(mergeCommand)) {
        return mergeMessages(jsons, parser.value(qmFile), parser.value(language));
    }

    parser.showHelp(1);
}
