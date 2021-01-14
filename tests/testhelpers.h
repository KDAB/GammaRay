/*
  testhelpers.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <qnamespace.h>

QT_BEGIN_NAMESPACE
template <typename T>
class QList;

class QString;
class QSignalSpy;
class QAbstractItemModel;
class QModelIndex;

typedef QList<QModelIndex> QModelIndexList;
QT_END_NAMESPACE

namespace TestHelpers
{
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

#define QVERIFY_RETURN_FALSE(statement) \
do {\
    if (!QTest::qVerify(static_cast<bool>(statement), #statement, "", __FILE__, __LINE__))\
    return false;\
} while (0)

#define QCOMPARE_RETURN_FALSE(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
    return false;\
} while (0)

#endif // TESTHELPERS_H
