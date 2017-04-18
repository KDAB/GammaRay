/*
  classesiconsextractor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDebug>

enum AppStatus {
    InvalidArgumentsCount = 1,
    InvalidQrc,
    InvalidOutput,
    InvalidXml,
    FatalError
};

struct DomError {
    DomError()
        : line(-1)
        , column(-1)
    {
    }

    QString toString() const
    {
        return QString::fromLatin1("An error occured at line %1, column %2: %3")
                .arg(line).arg(column).arg(error);
    }

    QString error;
    int line;
    int column;
};

static void usage() {
    printf("Classes icons extractor tool for common classes index.\n");
    printf("\n");
    printf("%s -qrc SourceFilePath -out TargetFilePath\n", qPrintable(qApp->applicationName()));
    printf("\n");
    printf("    -qrc\t\tThe input qrc file.\n");
    printf("    -out\tThe output file to generate.\n");
    printf("\n");
}

static int generateOutput(const QFileInfo &qrc, const QFileInfo &out) {
    QDomDocument document;

    {
        QFile qrcFile(qrc.absoluteFilePath());

        if (!qrcFile.open(QIODevice::ReadOnly)) {
            return InvalidQrc;
        }

        DomError error;

        if (!document.setContent(&qrcFile, &error.error, &error.line, &error.column)) {
            qWarning("%s", qPrintable(error.toString()));
            return InvalidXml;
        }
    }

    QFile outFile(out.absoluteFilePath());

    if (!outFile.open(QIODevice::WriteOnly) || !outFile.resize(0)) {
        return InvalidOutput;
    }

    QTextStream outStream(&outFile);
    outStream.setCodec("UTF-8");

    outStream << QLatin1String("// This is machine generated code, DO NOT EDIT.") << QLatin1Char('\n');
    outStream << QLatin1String("// This generated file is versionned only to easier crossbuilding to others architectures.") << QLatin1Char('\n');
    outStream << QLatin1String("// See classesiconsextractor.cpp to change this code.") << QLatin1Char('\n');
    outStream << QLatin1String("namespace GammaRay {") << QLatin1Char('\n');
    outStream << QLatin1String("namespace ClassesIconsIndex {") << QLatin1Char('\n');
    outStream << QLatin1String("static QHash<int, QString> s_numberToKey;") << QLatin1Char('\n');
    outStream << QLatin1String("static QHash<QString, int> s_keyToNumber;") << QLatin1Char('\n');
    outStream << QLatin1Char('\n');
    outStream << QLatin1String("static void initializeClassesIconsIndex()") << QLatin1Char('\n');
    outStream << QLatin1String("{") << QLatin1Char('\n');
    outStream << QLatin1String("    Q_ASSERT(s_numberToKey.isEmpty());") << QLatin1Char('\n');
    outStream << QLatin1String("    Q_ASSERT(s_keyToNumber.isEmpty());") << QLatin1Char('\n');
    {
        const QDomNodeList qresources = document.documentElement().elementsByTagName(QStringLiteral("qresource"));
        int y = 0;

        for (int i = 0; i < qresources.count(); ++i) {
            const QDomElement qresource(qresources.at(i).toElement());
            const QDomNodeList files = qresource.elementsByTagName(QStringLiteral("file"));

            for (int j = 0; j < files.count(); ++j) {
                const QDomElement file(files.at(j).toElement());

                if (file.attribute(QStringLiteral("alias")).startsWith(QStringLiteral("gammaray/icons/ui/classes"))) {
                    const QString filePath = QDir::cleanPath(QString::fromLatin1(":%1/%2")
                            .arg(qresource.attribute(QStringLiteral("prefix")))
                            .arg(file.attribute(QStringLiteral("alias"))));
                    outStream << QString::fromLatin1("    s_numberToKey[%1] = QLatin1String(\"%2\");\n").arg(y).arg(filePath);
                    outStream << QString::fromLatin1("    s_keyToNumber[s_numberToKey[%1]] = %1;\n").arg(y);
                    ++y;
                }
            }
        }
    }
    outStream << QLatin1String("}") << QLatin1Char('\n');
    outStream << QLatin1String("}") << QLatin1Char('\n');
    outStream << QLatin1String("}") << QLatin1Char('\n');

    return 0;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("classesiconsextractor");
    QCoreApplication::setApplicationVersion(GAMMARAY_VERSION_STRING);

    if (argc != 5 || argv[1] != QByteArray("-qrc") || argv[3] != QByteArray("-out")) {
        usage();
        return InvalidArgumentsCount;
    } else {
        const QFileInfo qrc(QString::fromLocal8Bit(argv[2]));
        const QFileInfo out(QString::fromLocal8Bit(argv[4]));
        return generateOutput(qrc, out);
    }
}
