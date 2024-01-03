/*
  quickinspectorbench.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include <plugins/quickinspector/quickitemmodel.h>

#include <QDebug>
#include <QQuickItem>
#include <QQuickView>
#include <QTest>

using namespace GammaRay;

class QuickInspectorBench : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
    }

    void benchModelObjectAdded()
    {
        QQuickView view;
        auto root = view.contentItem();
        QuickItemModel model;
        model.setWindow(&view);
        const auto items = createItems(root);

        QBENCHMARK_ONCE
        {
            for (auto item : items) {
                model.objectAdded(item);
            }
        }
    }

    void benchModelItemUpdated()
    {
        QQuickView view;
        auto root = view.contentItem();
        QuickItemModel model;
        model.setWindow(&view);
        const auto items = createItems(root);

        for (auto item : items) {
            model.objectAdded(item);
        }

        QBENCHMARK_ONCE
        {
            for (auto item : items) {
                // trigger item update
                item->setX(item->x() + 1);
            }
        }
    }

private:
    static QVector<QQuickItem *> createItems(QQuickItem *parent)
    {
        const int numberOfItems = 100; // increase when benchmarking as needed
        QVector<QQuickItem *> items;
        items.reserve(numberOfItems);
        for (int i = 0; i < numberOfItems; ++i) {
            items << new QQuickItem(parent);
        }
        return items;
    }
};

QTEST_MAIN(QuickInspectorBench)

#include "quickinspectorbench.moc"
