/*
  testhelpers.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
        spy->wait(5000);

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
