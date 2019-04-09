/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include <config-gammaray.h>

#include <plugins/quickinspector/quickitemmodel.h>

#include <QtTest/qtest.h>

#include <QDebug>
#include <QQuickItem>
#include <QQuickView>

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

        QBENCHMARK_ONCE {
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

        QBENCHMARK_ONCE {
            for (auto item : items) {
                // trigger item update
                item->setX(item->x() + 1);
            }
        }
    }

private:
    QVector<QQuickItem *> createItems(QQuickItem* parent)
    {
        const int numberOfItems = 10000;
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
