/*
  testhelpers.cpp

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

#include "testhelpers.h"

#include <QSignalSpy>
#include <QAbstractItemModel>
#include <QTest>

static QModelIndex searchMatchesOne(QAbstractItemModel *model, const QVariant &value,
                                    int role, int column, Qt::MatchFlags flags)
{
    const auto matches = model->match(model->index(0, column), role, value, 1, flags);

    if (matches.isEmpty())
        return {};

    const QModelIndex idx = matches.at(0);
    Q_ASSERT(idx.isValid());
    return idx;
}

static QModelIndexList searchMatchesAll(QAbstractItemModel *model, const QVariant &value,
                                        int role, int column, Qt::MatchFlags flags)
{
    const auto matches = model->match(model->index(0, column), role, value, -1, flags);

    for (const QModelIndex &idx : matches) {
        Q_UNUSED(idx);
        Q_ASSERT(idx.isValid());
    }

    return matches;
}

bool TestHelpers::waitForSignal(QSignalSpy *spy, bool keepResult)
{
    if (spy->isEmpty())
        spy->wait(1000);

    bool result = !spy->isEmpty();
    if (!keepResult)
        spy->clear();

    return result;
}

QModelIndex TestHelpers::searchFixedIndex(QAbstractItemModel *model, const QString &value,
                                          Qt::MatchFlags extra, int role, int column)
{
    return searchMatchesOne(model, value, role, column, Qt::MatchFixedString | extra);
}

QModelIndexList TestHelpers::searchFixedIndexes(QAbstractItemModel *model, const QString &value,
                                                Qt::MatchFlags extra, int role, int column)
{
    return searchMatchesAll(model, value, role, column, Qt::MatchFixedString | extra);
}

QModelIndex TestHelpers::searchContainsIndex(QAbstractItemModel *model, const QString &value,
                                             Qt::MatchFlags extra, int role, int column)
{
    return searchMatchesOne(model, value, role, column, Qt::MatchContains | extra);
}

QModelIndexList TestHelpers::searchContainsIndexes(QAbstractItemModel *model, const QString &value,
                                                   Qt::MatchFlags extra, int role, int column)
{
    return searchMatchesAll(model, value, role, column, Qt::MatchContains | extra);
}
