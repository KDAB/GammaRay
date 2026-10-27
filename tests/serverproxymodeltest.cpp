/*
  serverproxymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/remote/serverproxymodel.h>

#include <common/modelevent.h>

#include <QAbstractItemModelTester>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTest>

#include <memory>

using namespace GammaRay;

// remembers whether it was told that it is used
class RecordingModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit RecordingModel(int rows = 3)
    {
        for (int row = 0; row < rows; ++row)
            appendRow(new QStandardItem(QStringLiteral("item %1").arg(row)));
    }

    QVector<bool> events;

protected:
    void customEvent(QEvent *event) override
    {
        if (event->type() == ModelEvent::eventType())
            events.push_back(static_cast<ModelEvent *>(event)->used());
        QStandardItemModel::customEvent(event);
    }
};

// provides roles that QAbstractItemModel::itemData() doesn't collect
class ExtraRolesModel : public QStringListModel
{
    Q_OBJECT
public:
    ExtraRolesModel()
        : QStringListModel({ QStringLiteral("item 0") })
    {
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::UserRole + 1)
            return 42;
        if (role == Qt::UserRole + 2)
            return QStringLiteral("proxy");
        return QStringListModel::data(index, role);
    }
};

using Proxy = ServerProxyModel<QSortFilterProxyModel>;

class ServerProxyModelTest : public QObject
{
    Q_OBJECT
private:
    static void setUsed(QAbstractItemModel *model, bool used)
    {
        ModelEvent event(used);
        QCoreApplication::sendEvent(model, &event);
    }

private slots:
    // nothing is filtered or sorted for nobody
    static void testInactiveUntilUsed()
    {
        RecordingModel source;
        Proxy proxy;
        QAbstractItemModelTester tester(&proxy);
        proxy.setSourceModel(&source);
        QCOMPARE(proxy.sourceModel(), nullptr);
        QVERIFY(source.events.isEmpty());

        setUsed(&proxy, true);
        QCOMPARE(proxy.sourceModel(), &source);
        QCOMPARE(proxy.rowCount(), 3);
        QCOMPARE(source.events, QVector<bool>({ true }));

        setUsed(&proxy, false);
        QCOMPARE(proxy.sourceModel(), nullptr);
        QCOMPARE(proxy.rowCount(), 0);
        QCOMPARE(source.events, QVector<bool>({ true, false }));
    }

    // used in-process, without ModelEvents
    static void testIndexMakesItUsed()
    {
        RecordingModel source;
        Proxy proxy;
        proxy.setSourceModel(&source);
        proxy.index(0, 0);
        QCOMPARE(proxy.sourceModel(), &source);
        QCOMPARE(proxy.rowCount(), 3);
        QCOMPARE(source.events, QVector<bool>({ true }));
    }

    static void testSourceSetWhileUsed()
    {
        RecordingModel source;
        Proxy proxy;
        setUsed(&proxy, true);
        proxy.setSourceModel(&source);
        QCOMPARE(proxy.sourceModel(), &source);
        QCOMPARE(source.events, QVector<bool>({ true }));
    }

    // the new source is used now, the old one isn't anymore
    static void testSourceReplacedWhileUsed()
    {
        RecordingModel source;
        RecordingModel other(5);
        Proxy proxy;
        QAbstractItemModelTester tester(&proxy);
        proxy.setSourceModel(&source);
        setUsed(&proxy, true);

        proxy.setSourceModel(&other);
        QCOMPARE(proxy.sourceModel(), &other);
        QCOMPARE(proxy.rowCount(), 5);
        QCOMPARE(other.events, QVector<bool>({ true }));
        QCOMPARE(source.events, QVector<bool>({ true, false }));

        setUsed(&proxy, false);
        QCOMPARE(other.events, QVector<bool>({ true, false }));
        QCOMPARE(source.events, QVector<bool>({ true, false }));
    }

    static void testSourceClearedWhileUsed()
    {
        RecordingModel source;
        Proxy proxy;
        QAbstractItemModelTester tester(&proxy);
        proxy.setSourceModel(&source);
        setUsed(&proxy, true);

        proxy.setSourceModel(nullptr);
        QCOMPARE(proxy.sourceModel(), nullptr);
        QCOMPARE(proxy.rowCount(), 0);
        QCOMPARE(source.events, QVector<bool>({ true, false }));

        // and nothing to activate later
        setUsed(&proxy, false);
        setUsed(&proxy, true);
        QCOMPARE(proxy.sourceModel(), nullptr);
        QCOMPARE(source.events, QVector<bool>({ true, false }));
    }

    static void testSourceDeletedWhileUsed()
    {
        auto source = std::make_unique<RecordingModel>();
        Proxy proxy;
        QAbstractItemModelTester tester(&proxy);
        proxy.setSourceModel(source.get());
        setUsed(&proxy, true);
        QCOMPARE(proxy.rowCount(), 3);

        source.reset();
        QCOMPARE(proxy.rowCount(), 0);
        setUsed(&proxy, false);
        setUsed(&proxy, true);
        QCOMPARE(proxy.rowCount(), 0);
    }

    // itemData() passes on extra roles, of the source and of the proxy itself
    static void testItemData()
    {
        ExtraRolesModel source;
        Proxy proxy;
        proxy.setSourceModel(&source);
        setUsed(&proxy, true);

        const auto index = proxy.index(0, 0);
        QVERIFY(!proxy.itemData(index).contains(Qt::UserRole + 1));
        proxy.addRole(Qt::UserRole + 1);
        proxy.addProxyRole(Qt::UserRole + 2);
        const auto itemData = proxy.itemData(index);
        QCOMPARE(itemData.value(Qt::DisplayRole).toString(), QStringLiteral("item 0"));
        QCOMPARE(itemData.value(Qt::UserRole + 1).toInt(), 42);
        QCOMPARE(itemData.value(Qt::UserRole + 2).toString(), QStringLiteral("proxy"));
    }

    // trees are filtered keeping the matching items with their ancestors and descendants
    static void testRecursiveFiltering()
    {
        Proxy proxy;
        QVERIFY(proxy.isRecursiveFilteringEnabled());
        QVERIFY(proxy.autoAcceptChildRows());
    }
};

QTEST_GUILESS_MAIN(ServerProxyModelTest)

#include "serverproxymodeltest.moc"
