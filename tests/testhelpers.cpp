#include "testhelpers.h"

#include <QSignalSpy>
#include <QAbstractItemModel>
#include <QtTest/qtest.h>

static QModelIndex searchMatchesOne(QAbstractItemModel *model, const QVariant &value,
                                    int role, int column, Qt::MatchFlags flags)
{
    const auto matches = model->match(model->index(0, column), role, value, 1, flags);

    if (matches.size() < 1)
        return QModelIndex();

    const QModelIndex idx = matches.at(0);
    Q_ASSERT(idx.isValid());
    return idx;
}

static QModelIndexList searchMatchesAll(QAbstractItemModel *model, const QVariant &value,
                                        int role, int column, Qt::MatchFlags flags)
{
    const auto matches = model->match(model->index(0, column), role, value, -1, flags);

    foreach (const QModelIndex &idx, matches) {
        Q_UNUSED(idx);
        Q_ASSERT(idx.isValid());
    }

    return matches;
}

void TestHelpers::waitForSpy(QSignalSpy *spy, int timeout)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    spy->wait(timeout);
#else
    int loops = 0;
    while (loops++ < (timeout / 10)) {
        if (spy->count() >= 1)
            break;
        QTest::qWait(10);
    }
#endif
}

bool TestHelpers::waitForSignal(QSignalSpy *spy, bool keepResult)
{
    if (spy->isEmpty())
        waitForSpy(spy, 1000);

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
