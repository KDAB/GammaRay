/*
  quickscenegraphmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <plugins/quickinspector/quickscenegraphmodel.h>

#include <QAbstractItemModelTester>
#include <QQuickItem>
#include <QQuickView>
#include <QSet>
#include <QSGNode>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QTest>

#include <private/qquickitem_p.h>
#include <private/qquickrectangle_p.h>

#include <algorithm>
#include <memory>

Q_DECLARE_METATYPE(QSGNode *)

using namespace GammaRay;

class QuickSceneGraphModelTest : public QObject
{
    Q_OBJECT
private:
    static QVector<QSGNode *> sortedChildren(QSGNode *node)
    {
        QVector<QSGNode *> children;
        for (QSGNode *child = node->firstChild(); child; child = child->nextSibling())
            children.append(child);
        std::sort(children.begin(), children.end());
        return children;
    }

    QSGNode *rootNode() const
    {
        QSGNode *root = QQuickItemPrivate::get(m_view->contentItem())->itemNodeInstance;
        while (root && root->parent())
            root = root->parent();
        return root;
    }

    static QSGNode *itemNode(QQuickItem *item)
    {
        return QQuickItemPrivate::get(item)->itemNodeInstance;
    }

    static QQuickItem *findItem(QQuickItem *parent, const QString &name)
    {
        for (auto child : parent->childItems()) {
            if (child->objectName() == name)
                return child;
            if (auto item = findItem(child, name))
                return item;
        }
        return nullptr;
    }

    // Repeater delegates are not QObject children of the root item
    QQuickItem *item(const QString &name) const
    {
        return findItem(m_view->contentItem(), name);
    }

    bool compareWithScene(const QModelIndex &index, QSGNode *node) const
    {
        const auto children = sortedChildren(node);
        if (m_model->rowCount(index) != children.size()) {
            qWarning() << "row count mismatch for" << node << m_model->rowCount(index) << children.size();
            return false;
        }
        for (int row = 0; row < children.size(); ++row) {
            const auto childIndex = m_model->index(row, 0, index);
            if (childIndex.internalPointer() != children.at(row) || m_model->indexForNode(children.at(row)) != childIndex) {
                qWarning() << "wrong node in row" << row << "of" << node;
                return false;
            }
            if (!compareWithScene(childIndex, children.at(row)))
                return false;
        }
        return true;
    }

    // the model contains exactly the current scene graph
    bool modelMatchesScene() const
    {
        QSGNode *root = rootNode();
        if (!root || m_model->rowCount() != 1 || m_model->index(0, 0, {}).internalPointer() != root)
            return false;
        return compareWithScene(m_model->index(0, 0, {}), root);
    }

    // walks the whole proxy, so the next change has to update mappings for all of it
    static bool compareProxies(const QAbstractItemModel *a, const QModelIndex &aParent,
                               const QSortFilterProxyModel *b, const QModelIndex &bParent)
    {
        const int rows = a->rowCount(aParent);
        if (rows != b->rowCount(bParent)) {
            qWarning() << "proxy row count mismatch" << rows << b->rowCount(bParent);
            return false;
        }
        for (int row = 0; row < rows; ++row) {
            const auto aIndex = a->index(row, 0, aParent);
            const auto bIndex = b->index(row, 0, bParent);
            const auto aSource = qobject_cast<const QSortFilterProxyModel *>(a) ? static_cast<const QSortFilterProxyModel *>(a)->mapToSource(aIndex) : aIndex;
            if (aSource != b->mapToSource(bIndex)) {
                qWarning() << "proxy maps row" << row << "to a different node";
                return false;
            }
            if (!compareProxies(a, aIndex, b, bIndex))
                return false;
        }
        return true;
    }

    // the model matches the scene and the proxies that already existed during the change are still
    // correct, the filtered one is compared with a fresh proxy using the same filter
    bool verify() const
    {
        if (!modelMatchesScene())
            return false;
        if (!compareProxies(m_model.get(), {}, m_proxy.get(), {}))
            return false;
        QSortFilterProxyModel freshProxy;
        setupFilteredProxy(&freshProxy);
        return compareProxies(&freshProxy, {}, m_filteredProxy.get(), {});
    }

    void setupFilteredProxy(QSortFilterProxyModel *proxy) const
    {
        proxy->setRecursiveFilteringEnabled(true);
        proxy->setFilterKeyColumn(1);
        proxy->setFilterFixedString(QStringLiteral("Clip"));
        proxy->setSourceModel(m_model.get());
    }

    int checkItemForNodes(QSGNode *node, QQuickItem *quickItem, const QSet<QSGNode *> &childItemNodes) const
    {
        int count = 0;
        for (QSGNode *child = node->firstChild(); child; child = child->nextSibling()) {
            if (childItemNodes.contains(child))
                continue;
            if (m_model->itemForSgNode(child) != quickItem)
                return -1;
            const int childCount = checkItemForNodes(child, quickItem, childItemNodes);
            if (childCount < 0)
                return -1;
            count += 1 + childCount;
        }
        return count;
    }

    bool waitForFrame()
    {
        QSignalSpy spy(m_view.get(), &QQuickWindow::frameSwapped);
        return spy.wait();
    }

private slots:
    static void initTestCase()
    {
        qRegisterMetaType<QSGNode *>();
    }

    void init()
    {
        m_view = std::make_unique<QQuickView>();
        m_view->setResizeMode(QQuickView::SizeViewToRootObject);
        m_view->setSource(QUrl(QStringLiteral("qrc:/manual/sgnodemovetest.qml")));
        QVERIFY(m_view->rootObject());
        m_view->show();
        QVERIFY(QTest::qWaitForWindowExposed(m_view.get()));
        QTRY_VERIFY(rootNode());

        m_model = std::make_unique<QuickSceneGraphModel>();
        new QAbstractItemModelTester(m_model.get(), m_model.get());
        m_model->setWindow(m_view.get());

        m_proxy = std::make_unique<QSortFilterProxyModel>();
        m_proxy->setRecursiveFilteringEnabled(true);
        m_proxy->setSourceModel(m_model.get());
        new QAbstractItemModelTester(m_proxy.get(), m_proxy.get());

        m_filteredProxy = std::make_unique<QSortFilterProxyModel>();
        setupFilteredProxy(m_filteredProxy.get());
        new QAbstractItemModelTester(m_filteredProxy.get(), m_filteredProxy.get());

        QVERIFY(verify());
    }

    void cleanup()
    {
        m_filteredProxy.reset();
        m_proxy.reset();
        m_model.reset();
        m_view.reset();
    }

    void testNodeLookup()
    {
        for (const auto &name : { QStringLiteral("container"), QStringLiteral("rect0"), QStringLiteral("innerRect5") }) {
            auto quickItem = item(name);
            QVERIFY(quickItem);
            QSGNode *node = itemNode(quickItem);
            QVERIFY(node);
            QCOMPARE(m_model->sgNodeForItem(quickItem), node);
            QCOMPARE(m_model->itemForSgNode(node), quickItem);
            QVERIFY(m_model->indexForNode(node).isValid());
            QCOMPARE(m_model->indexForNode(node).internalPointer(), node);

            // the nodes below the item node belong to it, up to the item nodes of its children
            QSet<QSGNode *> childItemNodes;
            for (auto childItem : quickItem->childItems())
                childItemNodes.insert(itemNode(childItem));
            const int nodeCount = checkItemForNodes(node, quickItem, childItemNodes);
            QVERIFY(nodeCount >= 0);
            if (qobject_cast<QQuickRectangle *>(quickItem))
                QVERIFY(nodeCount > 0);
        }
        QVERIFY(!m_model->indexForNode(nullptr).isValid());
        QCOMPARE(m_model->itemForSgNode(nullptr), nullptr);
    }

    static void testWrapperNodes_data()
    {
        QTest::addColumn<bool>("clip");
        QTest::addColumn<qreal>("opacity");

        QTest::newRow("clip") << true << 1.0;
        QTest::newRow("opacity") << false << 0.5;
        QTest::newRow("clip and opacity") << true << 0.5;
    }

    // a clip or opacity node is inserted between an item node and the nodes that existed below it
    void testWrapperNodes()
    {
        QFETCH(bool, clip);
        QFETCH(qreal, opacity);

        for (const auto &name : { QStringLiteral("container"), QStringLiteral("rect2") }) {
            auto quickItem = item(name);
            QVERIFY(quickItem);
            const int rowCount = m_model->rowCount(m_model->indexForNode(itemNode(quickItem)));

            quickItem->setClip(clip);
            quickItem->setOpacity(opacity);
            QVERIFY(waitForFrame());
            QTRY_VERIFY(verify());
            QCOMPARE(m_model->rowCount(m_model->indexForNode(itemNode(quickItem))), 1);

            // Qt keeps an opacity node once it exists
            quickItem->setClip(false);
            quickItem->setOpacity(1.0);
            QVERIFY(waitForFrame());
            QTRY_VERIFY(verify());
            if (opacity == 1.0)
                QCOMPARE(m_model->rowCount(m_model->indexForNode(itemNode(quickItem))), rowCount);
        }
    }

    void testReparent()
    {
        auto container = item(QStringLiteral("container"));
        auto container2 = item(QStringLiteral("container2"));
        auto rect = item(QStringLiteral("rect1"));
        auto innerRect = item(QStringLiteral("innerRect3"));
        QVERIFY(container && container2 && rect && innerRect);

        QSGNode *rectNode = itemNode(rect);
        rect->setParentItem(container2);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
        QCOMPARE(m_model->itemForSgNode(rectNode), rect);

        rect->setParentItem(container);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());

        // to an ancestor, and back down again
        innerRect->setParentItem(container);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
        innerRect->setParentItem(rect);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());

        // a whole subtree, and several items in the same frame
        container2->setParentItem(rect);
        item(QStringLiteral("rect4"))->setParentItem(container2);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
    }

    // existing nodes end up below a node that is new to the model
    void testReparentIntoNewItem()
    {
        auto container2 = item(QStringLiteral("container2"));
        QVERIFY(container2);

        auto newItem = new QQuickItem(container2);
        auto newChild = new QQuickItem(newItem);
        newChild->setClip(true);
        for (const auto &name : { QStringLiteral("rect0"), QStringLiteral("rect3"), QStringLiteral("innerRect5") })
            item(name)->setParentItem(newChild);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());

        delete newItem;
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
    }

    void testCreateDestroy()
    {
        auto repeater = m_view->rootObject()->findChild<QObject *>(QStringLiteral("repeater"));
        QVERIFY(repeater);

        repeater->setProperty("model", 10);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
        QVERIFY(item(QStringLiteral("innerRect9")));

        repeater->setProperty("model", 2);
        QTRY_VERIFY(!item(QStringLiteral("rect7")));
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());

        repeater->setProperty("model", 6);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
    }

    void testNodeDeleted()
    {
        auto rect = item(QStringLiteral("rect2"));
        QVERIFY(rect);
        QSGNode *node = itemNode(rect);
        QSignalSpy deletedSpy(m_model.get(), &QuickSceneGraphModel::nodeDeleted);

        // only the parent of the removed subtree is reported, nothing new is created in this frame,
        // so the address can't be taken by another node meanwhile
        delete rect;
        QTRY_COMPARE(deletedSpy.size(), 1);
        QCOMPARE(deletedSpy.at(0).at(0).value<QSGNode *>(), node);
        QVERIFY(verify());

        // the lookups don't know the deleted item anymore, the pointers are only compared
        QCOMPARE(m_model->sgNodeForItem(rect), nullptr);
        QVERIFY(!m_model->indexForNode(node).isValid());
    }

    void testWindowChange()
    {
        m_model->setWindow(nullptr);
        QCOMPARE(m_model->rowCount(), 0);
        QCOMPARE(m_proxy->rowCount(), 0);

        m_model->setWindow(m_view.get());
        QVERIFY(verify());

        // updates continue after a reset
        item(QStringLiteral("container"))->setClip(true);
        QVERIFY(waitForFrame());
        QTRY_VERIFY(verify());
    }

private:
    std::unique_ptr<QQuickView> m_view;
    std::unique_ptr<QuickSceneGraphModel> m_model;
    std::unique_ptr<QSortFilterProxyModel> m_proxy;
    std::unique_ptr<QSortFilterProxyModel> m_filteredProxy;
};

QTEST_MAIN(QuickSceneGraphModelTest)

#include "quickscenegraphmodeltest.moc"
