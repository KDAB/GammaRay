/*
  favoritesmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <ui/favoritesitemview.h>

#include <common/objectmodel.h>

#include <QAbstractItemModelTester>
#include <QStandardItemModel>
#include <QTest>

using namespace GammaRay;

class FavoritesModelTest : public QObject
{
    Q_OBJECT
private:
    static QStandardItem *item(const QString &text, bool favorite = false)
    {
        auto item = new QStandardItem(text);
        item->setData(favorite, ObjectModel::IsFavoriteRole);
        return item;
    }

    static QString treeString(const QAbstractItemModel *model, const QModelIndex &parent = {})
    {
        QStringList rows;
        for (int row = 0; row < model->rowCount(parent); ++row) {
            const auto index = model->index(row, 0, parent);
            auto text = index.data().toString();
            if (model->rowCount(index) > 0)
                text += QLatin1Char('[') + treeString(model, index) + QLatin1Char(']');
            rows.push_back(text);
        }
        return rows.join(QLatin1Char(' '));
    }

private slots:
    // mapping a row below a hidden parent doesn't break showing that parent later, QTBUG-76976
    static void testMapFromSourceBelowHiddenParent()
    {
        QStandardItemModel source;
        auto root = item(QStringLiteral("root"));
        source.appendRow(root);
        root->appendRow(item(QStringLiteral("favorite"), true));
        auto shared = item(QStringLiteral("shared"));
        root->appendRow(shared);
        auto hidden = item(QStringLiteral("hidden"));
        shared->appendRow(hidden);

        // like ObjectsFavoriteView sets it up
        FavoritesModel proxy(&source);
        proxy.setRecursiveFilteringEnabled(true);
        QAbstractItemModelTester tester(&proxy);
        QCOMPARE(treeString(&proxy), QStringLiteral("root[favorite]"));

        QVERIFY(!proxy.mapFromSource(hidden->index()).isValid());

        // a favorite child makes the parent appear
        const auto insertFavorite = [shared] {
            shared->insertRow(0, item(QStringLiteral("favorite2"), true));
        };
        insertFavorite();
        QCOMPARE(treeString(&proxy), QStringLiteral("root[favorite shared[favorite2]]"));
        shared->removeRow(0);
        QCOMPARE(treeString(&proxy), QStringLiteral("root[favorite]"));
        insertFavorite();
        QCOMPARE(treeString(&proxy), QStringLiteral("root[favorite shared[favorite2]]"));
    }
};

QTEST_MAIN(FavoritesModelTest)

#include "favoritesmodeltest.moc"
