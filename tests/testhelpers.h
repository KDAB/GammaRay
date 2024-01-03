/*
  testhelpers.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <qnamespace.h>
#include <functional>

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

class kdScopeGuard
{
public:
    kdScopeGuard(std::function<void()> cb)
        : m_cb(cb)
    {
    }
    ~kdScopeGuard()
    {
        if (m_cb) {
            m_cb();
        }
    }

private:
    std::function<void()> m_cb;
};
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
