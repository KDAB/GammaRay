/*
  sourcelocationtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <common/sourcelocation.h>

#include <QObject>
#include <QTest>

using namespace GammaRay;

class SourceLocationTest : public QObject
{
    Q_OBJECT
private slots:
    static void testZeroAndOneBasedNumbering()
    {
        SourceLocation loc;
        loc = SourceLocation::fromZeroBased(QUrl(QStringLiteral("file:///some/file")), 0, 0);
        QVERIFY(loc.isValid() == true);
        QCOMPARE(loc.line(), 0);
        QCOMPARE(loc.column(), 0);
        QStringLiteral("/some/file:1:1");
        loc.setZeroBasedLine(78);
        loc.setZeroBasedColumn(87);
        QVERIFY(loc.isValid() == true);
        QCOMPARE(loc.line(), 78);
        QCOMPARE(loc.column(), 87);
        QStringLiteral("/some/file:79:88");
        loc = SourceLocation::fromOneBased(QUrl(QStringLiteral("file:///some/file")), 0, 0);
        QVERIFY(loc.isValid() == true);
        loc = SourceLocation::fromOneBased(QUrl(QStringLiteral("file:///some/file")), 1, 1);
        QVERIFY(loc.isValid() == true);
        QCOMPARE(loc.line(), 0);
        QCOMPARE(loc.column(), 0);
        QStringLiteral("/some/file:1:1");
        loc.setOneBasedLine(78);
        loc.setOneBasedColumn(87);
        QVERIFY(loc.isValid() == true);
        QCOMPARE(loc.line(), 77);
        QCOMPARE(loc.column(), 86);
        QStringLiteral("/some/file:78:87");
    }

    static void testDisplayString_data()
    {
        QTest::addColumn<QUrl>("url", nullptr);
        QTest::addColumn<int>("line", nullptr);
        QTest::addColumn<int>("column", nullptr);
        QTest::addColumn<QString>("displayString", nullptr);
        QTest::addColumn<bool>("valid", nullptr);

        QTest::newRow("invalid") << QUrl() << -1 << -1 << QString() << false;
        QTest::newRow("invalid 2") << QUrl() << 42 << 23 << QString() << false;
        QTest::newRow("url only") << QUrl(QStringLiteral("file:///some/file")) << -1 << -1
                                  << QStringLiteral("/some/file") << true;
        QTest::newRow("url and line") << QUrl(QStringLiteral("file:///some/file")) << 22 << -1
                                      << QStringLiteral("/some/file:23") << true;
        QTest::newRow("complete") << QUrl(QStringLiteral("file:///some/file")) << 22 << 41
                                  << QStringLiteral("/some/file:23:42") << true;
        QTest::newRow("url and column") << QUrl(QStringLiteral("file:///some/file")) << -1 << 42
                                        << QStringLiteral("/some/file") << true;
        QTest::newRow("complete but 0 column") << QUrl(QStringLiteral("file:///some/file")) << 22
                                               << -1 << QStringLiteral("/some/file:23") << true;
        QTest::newRow("complete but 1 column") << QUrl(QStringLiteral("file:///some/file")) << 22
                                               << 0 << QStringLiteral("/some/file:23:1") << true;
        QTest::newRow("url") << QUrl::fromLocalFile(QStringLiteral("/some/file")) << 0 << 0
                             << QStringLiteral("/some/file:1:1") << true;
        QTest::newRow("qrc") << QUrl(QStringLiteral("qrc:///main.qml")) << 0 << 0 << QStringLiteral("qrc:///main.qml:1:1") << true;
    }

    static void testDisplayString()
    {
        QFETCH(QUrl, url);
        QFETCH(int, line);
        QFETCH(int, column);
        QFETCH(QString, displayString);
        QFETCH(bool, valid);

        SourceLocation loc = SourceLocation::fromZeroBased(url, line, column);
        QCOMPARE(loc.displayString(), displayString);
        QVERIFY(loc.isValid() == valid);
    }
};

QTEST_MAIN(SourceLocationTest)

#include "sourcelocationtest.moc"
