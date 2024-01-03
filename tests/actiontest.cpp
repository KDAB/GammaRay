/*
  actiontest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"
#include "testhelpers.h"

#include <plugins/actioninspector/clientactionmodel.h>

#include <common/objectbroker.h>

#include <QAbstractItemModel>
#include <QAction>
#include <QSignalSpy>

using namespace GammaRay;
using namespace TestHelpers;

class ActionTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    void testActionCreationDeletion()
    {
        createProbe();

        std::unique_ptr<QAction> a1(new QAction(QStringLiteral("Action 1"), this));
        a1->setObjectName("action1");
        std::unique_ptr<QAction> a2(new QAction(QStringLiteral("Action 2"), this));
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        QVERIFY(sourceModel);
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QCOMPARE(model->rowCount(), 2);

        auto idx = searchFixedIndex(model, "action1");
        QVERIFY(idx.isValid());
        idx = idx.sibling(idx.row(), 5);
        QVERIFY(idx.data().toString().isEmpty());
        QVERIFY(idx.data(Qt::ToolTipRole).isNull());
        QVERIFY(idx.data(Qt::DecorationRole).isNull());

        a1.reset();
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(model->rowCount(), 1);

        a2.reset();
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(model->rowCount(), 0);
    }

    void testConflictDetection()
    {
        createProbe();

        std::unique_ptr<QAction> a1(new QAction(QStringLiteral("Action 1"), this));
        a1->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        a1->setShortcutContext(Qt::ApplicationShortcut);
        std::unique_ptr<QAction> a2(new QAction(QStringLiteral("Action 2"), this));
        a2->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        a2->setShortcutContext(Qt::WidgetShortcut);
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        QVERIFY(sourceModel);
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QVERIFY(model);
        QCOMPARE(model->rowCount(), 2);

        const auto index = model->index(0, 5);
        QCOMPARE(index.data(Qt::DisplayRole).toString(), QKeySequence(QStringLiteral("Ctrl+K")).toString(QKeySequence::PortableText));
        QVERIFY(!index.data(Qt::DecorationRole).isNull());
        QVERIFY(!index.data(Qt::ToolTipRole).toString().isEmpty());

        a1.reset();
        a2.reset();
        QTest::qWait(1);
    }

    void testActionChanges()
    {
        createProbe();

        std::unique_ptr<QAction> a1(new QAction(QStringLiteral("Action 1"), this));
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QVERIFY(model);
        QCOMPARE(model->rowCount(), 1);

        QSignalSpy changeSpy(model, &QAbstractItemModel::dataChanged);
        QVERIFY(changeSpy.isValid());

        a1->setCheckable(true);
        QCOMPARE(changeSpy.size(), 1);
        a1->setText("Renamed Action");
        QCOMPARE(changeSpy.size(), 2);

        a1->setChecked(true);
        QCOMPARE(changeSpy.size(), 3);

        a1->setDisabled(true);
        QCOMPARE(model->index(0, 0).data(Qt::CheckStateRole).toInt(), ( int )Qt::Unchecked);
        a1->setEnabled(true);
        QCOMPARE(model->index(0, 0).data(Qt::CheckStateRole).toInt(), ( int )Qt::Checked);
        QVERIFY(changeSpy.size() >= 4);
        changeSpy.clear();

        QVERIFY(model->setData(model->index(0, 0), Qt::Unchecked, Qt::CheckStateRole));
        QCOMPARE(a1->isEnabled(), false);
        QCOMPARE(changeSpy.size(), 1);

        a1->setChecked(true);
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(model->index(0, 3).data(Qt::CheckStateRole).toInt(), ( int )Qt::Checked);
        QVERIFY(model->setData(model->index(0, 3), Qt::Unchecked, Qt::CheckStateRole));
        QCOMPARE(a1->isChecked(), false);
        QCOMPARE(changeSpy.size(), 2);
    }
};

QTEST_MAIN(ActionTest)

#include "actiontest.moc"
