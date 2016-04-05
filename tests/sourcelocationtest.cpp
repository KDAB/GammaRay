/*
  sourcelocationtest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/sourcelocation.h>

#include <QtTest/qtest.h>
#include <QObject>

using namespace GammaRay;

class SourceLocationTest : public QObject
{
    Q_OBJECT
private slots:
    void testDisplayString_data()
    {
        QTest::addColumn<QString>("file");
        QTest::addColumn<int>("line");
        QTest::addColumn<int>("column");
        QTest::addColumn<QString>("displayString");

        QTest::newRow("invalid") << QString() << -1 << -1 << QString();
        QTest::newRow("invalid 2") << QString() << 42 << 23 << QString();
        QTest::newRow("file only") << QStringLiteral("/some/file") << -1 << -1 << QStringLiteral("/some/file");
        QTest::newRow("file and line") << QStringLiteral("/some/file") << 23 << -1 << QStringLiteral("/some/file:23");
        QTest::newRow("complete") << QStringLiteral("/some/file") << 23 << 42 << QStringLiteral("/some/file:23:42");
        QTest::newRow("file and column") << QStringLiteral("/some/file") << -1 << 42 << QStringLiteral("/some/file");
        QTest::newRow("complete but 0 column") << QStringLiteral("/some/file") << 23 << 0 << QStringLiteral("/some/file:23");
        QTest::newRow("complete but 1 column") << QStringLiteral("/some/file") << 23 << 1 << QStringLiteral("/some/file:23");
    }

    void testDisplayString()
    {
        QFETCH(QString, file);
        QFETCH(int, line);
        QFETCH(int, column);
        QFETCH(QString, displayString);

        SourceLocation loc(file, line, column);
        QCOMPARE(loc.displayString(), displayString);
    }

    void testDisplayStringForUrl_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<QString>("displayString");

        QTest::newRow("invalid") << QUrl() << QString();
        QTest::newRow("file") << QUrl::fromLocalFile(QStringLiteral("/some/file")) << QStringLiteral("/some/file");
        QTest::newRow("qrc") << QUrl(QStringLiteral("qrc:/main.qml")) << QStringLiteral("qrc:/main.qml");
    }

    void testDisplayStringForUrl()
    {
        QFETCH(QUrl, url);
        QFETCH(QString, displayString);

        SourceLocation loc(url);
        QCOMPARE(loc.displayString(), displayString);
    }
};

QTEST_MAIN(SourceLocationTest)

#include "sourcelocationtest.moc"
