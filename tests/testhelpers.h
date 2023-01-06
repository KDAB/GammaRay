/*
  testhelpers.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <qnamespace.h>

QT_BEGIN_NAMESPACE
template<typename T>
class QList;

class QString;
class QSignalSpy;
class QAbstractItemModel;
class QModelIndex;

typedef QList<QModelIndex> QModelIndexList;
QT_END_NAMESPACE

namespace TestHelpers {
bool waitForSignal(QSignalSpy *spy, bool keepResult = false);

QModelIndex searchFixedIndex(QAbstractItemModel *model, const QString &value,
                             Qt::MatchFlags extra = Qt::MatchExactly,
                             int role = Qt::DisplayRole, int column = 0);

QModelIndexList searchFixedIndexes(QAbstractItemModel *model, const QString &value,
                                   Qt::MatchFlags extra = Qt::MatchExactly,
                                   int role = Qt::DisplayRole, int column = 0);

QModelIndex searchContainsIndex(QAbstractItemModel *model, const QString &value,
                                Qt::MatchFlags extra = Qt::MatchExactly,
                                int role = Qt::DisplayRole, int column = 0);

QModelIndexList searchContainsIndexes(QAbstractItemModel *model, const QString &value,
                                      Qt::MatchFlags extra = Qt::MatchExactly,
                                      int role = Qt::DisplayRole, int column = 0);
}

#define QVERIFY_RETURN_FALSE(statement)                                                        \
    do {                                                                                       \
        if (!QTest::qVerify(static_cast<bool>(statement), #statement, "", __FILE__, __LINE__)) \
            return false;                                                                      \
    } while (0)

#define QCOMPARE_RETURN_FALSE(actual, expected)                                         \
    do {                                                                                \
        if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__)) \
            return false;                                                               \
    } while (0)

#endif // TESTHELPERS_H
