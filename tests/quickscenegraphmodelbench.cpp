/*
  quickscenegraphmodelbench.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <plugins/quickinspector/quickscenegraphmodel.h>

#include <QElapsedTimer>
#include <QQuickItem>
#include <QQuickView>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QTest>

#include <private/qquickrectangle_p.h>

#include <memory>

using namespace GammaRay;

class QuickSceneGraphModelBench : public QObject
{
    Q_OBJECT
private:
    void createScene(int groups, int itemsPerGroup)
    {
        m_groups.clear();
        for (int i = 0; i < groups; ++i) {
            auto group = new QQuickItem(m_view->contentItem());
            for (int j = 0; j < itemsPerGroup; ++j) {
                auto rect = new QQuickRectangle(group);
                rect->setSize({ 10, 10 });
                rect->setColor(Qt::red);
                auto inner = new QQuickRectangle(rect);
                inner->setSize({ 5, 5 });
                inner->setColor(Qt::blue);
            }
            m_groups.push_back(group);
        }
    }

    bool renderFrame()
    {
        QSignalSpy spy(m_view.get(), &QQuickWindow::frameSwapped);
        m_view->update();
        return spy.wait();
    }

    void updateModel()
    {
        QMetaObject::invokeMethod(m_model.get(), "updateSGTree", Q_ARG(bool, true));
    }

    // QBENCHMARK can't exclude the rendering of the changes, so time the model update ourselves
    template<typename F>
    void timeChanges(F change)
    {
        const int iterations = 10;
        qint64 total = 0;
        for (int i = 0; i < iterations; ++i) {
            change(i);
            QVERIFY(renderFrame());
            QElapsedTimer timer;
            timer.start();
            updateModel();
            total += timer.nsecsElapsed();
        }
        QTest::setBenchmarkResult(total / 1000000.0 / iterations, QTest::WalltimeMilliseconds);
    }

    static void walk(QAbstractItemModel *model, const QModelIndex &parent = {})
    {
        for (int row = 0; row < model->rowCount(parent); ++row)
            walk(model, model->index(row, 0, parent));
    }

    // the model only follows the scene when we tell it to
    void setupModel(bool withProxies)
    {
        m_model = std::make_unique<QuickSceneGraphModel>();
        m_model->setWindow(m_view.get());
        disconnect(m_view.get(), nullptr, m_model.get(), nullptr);
        if (withProxies) {
            m_proxy = std::make_unique<QSortFilterProxyModel>();
            m_proxy->setRecursiveFilteringEnabled(true);
            m_proxy->setSourceModel(m_model.get());
            walk(m_proxy.get());
            m_filteredProxy = std::make_unique<QSortFilterProxyModel>();
            m_filteredProxy->setRecursiveFilteringEnabled(true);
            m_filteredProxy->setFilterKeyColumn(1);
            m_filteredProxy->setFilterFixedString(QStringLiteral("Clip"));
            m_filteredProxy->setSourceModel(m_model.get());
            walk(m_filteredProxy.get());
        }
    }

private slots:
    void init()
    {
        m_view = std::make_unique<QQuickView>();
        m_view->resize(400, 400);
        // increase when benchmarking as needed, 50 groups with 100 items give a scene with about 20k nodes
        createScene(10, 50);
        m_view->show();
        QVERIFY(QTest::qWaitForWindowExposed(m_view.get()));
        QVERIFY(renderFrame());
    }

    void cleanup()
    {
        m_filteredProxy.reset();
        m_proxy.reset();
        m_model.reset();
        m_view.reset();
    }

    void benchSetWindow()
    {
        QBENCHMARK
        {
            QuickSceneGraphModel model;
            model.setWindow(m_view.get());
        }
    }

    static void benchUnchanged_data()
    {
        QTest::addColumn<bool>("withProxies");
        QTest::newRow("model") << false;
        QTest::newRow("with proxies") << true;
    }

    // runs after every frame
    void benchUnchanged()
    {
        QFETCH(bool, withProxies);
        setupModel(withProxies);
        QBENCHMARK
        {
            updateModel();
        }
    }

    static void benchWrapperNodes_data()
    {
        benchUnchanged_data();
    }

    // every group gets a clip node above all its children, and loses it again
    void benchWrapperNodes()
    {
        QFETCH(bool, withProxies);
        setupModel(withProxies);
        timeChanges([this](int iteration) {
            for (auto group : m_groups)
                group->setClip(iteration % 2 == 0);
        });
    }

    static void benchCreateDestroy_data()
    {
        benchUnchanged_data();
    }

    // half of the groups lose all their items, and get them back
    void benchCreateDestroy()
    {
        QFETCH(bool, withProxies);
        setupModel(withProxies);
        const int itemsPerGroup = m_groups.front()->childItems().size();
        timeChanges([this, itemsPerGroup](int iteration) {
            for (std::size_t i = 0; i < m_groups.size(); i += 2) {
                if (iteration % 2 == 0) {
                    qDeleteAll(m_groups.at(i)->childItems());
                } else {
                    for (int j = 0; j < itemsPerGroup; ++j) {
                        auto rect = new QQuickRectangle(m_groups.at(i));
                        rect->setSize({ 10, 10 });
                        new QQuickRectangle(rect);
                    }
                }
            }
        });
    }

    // half of the groups move into the other half
    void benchReparent_data()
    {
        benchUnchanged_data();
    }

    void benchReparent()
    {
        QFETCH(bool, withProxies);
        setupModel(withProxies);
        timeChanges([this](int iteration) {
            for (std::size_t i = 1; i < m_groups.size(); i += 2)
                m_groups.at(i)->setParentItem(iteration % 2 == 0 ? m_groups.at(i - 1) : m_view->contentItem());
        });
    }

private:
    std::unique_ptr<QQuickView> m_view;
    std::unique_ptr<QuickSceneGraphModel> m_model;
    std::unique_ptr<QSortFilterProxyModel> m_proxy;
    std::unique_ptr<QSortFilterProxyModel> m_filteredProxy;
    std::vector<QQuickItem *> m_groups;
};

QTEST_MAIN(QuickSceneGraphModelBench)

#include "quickscenegraphmodelbench.moc"
