/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <plugins/actioninspector/clientactionmodel.h>

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/paths.h>
#include <common/objectbroker.h>

#include <QtTest/qtest.h>

#include <QAbstractItemModel>
#include <QAction>
#include <QObject>
#include <QSignalSpy>

using namespace GammaRay;

class ActionTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
    }

    QModelIndex indexForName(const QString &name, QAbstractItemModel *model)
    {
        for (int i = 0; i < model->rowCount(); ++i) {
            const auto idx = model->index(i, 0);
            if (idx.data().toString() == name)
                return idx;
        }
        return QModelIndex();
    }

private slots:
    void testActionCreationDeletion()
    {
        createProbe();

        QAction *a1 = new QAction(QStringLiteral("Action 1"), this);
        a1->setObjectName("action1");
        QAction *a2 = new QAction(QStringLiteral("Action 2"), this);
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        QVERIFY(sourceModel);
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QCOMPARE(model->rowCount(), 2);

        auto idx = indexForName("action1", model);
        QVERIFY(idx.isValid());
        idx = idx.sibling(idx.row(), 5);
        QVERIFY(idx.data().toString().isEmpty());
        QVERIFY(idx.data(Qt::ToolTipRole).isNull());
        QVERIFY(idx.data(Qt::DecorationRole).isNull());

        delete a1;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(model->rowCount(), 1);

        delete a2;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(model->rowCount(), 0);
    }

    void testConflictDetection()
    {
        createProbe();

        QAction *a1 = new QAction(QStringLiteral("Action 1"), this);
        a1->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        QAction *a2 = new QAction(QStringLiteral("Action 2"), this);
        a2->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        QVERIFY(sourceModel);
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QVERIFY(model);
        QCOMPARE(model->rowCount(), 2);

        const auto index = model->index(0, 5);
        QCOMPARE(index.data(Qt::DisplayRole).toString(), QKeySequence(QStringLiteral("Ctrl+K")).toString( QKeySequence::PortableText));
        QVERIFY(!index.data(Qt::DecorationRole).isNull());
        QVERIFY(!index.data(Qt::ToolTipRole).toString().isEmpty());

        delete a1;
        delete a2;
        QTest::qWait(1);
    }

    void testActionChanges()
    {
        createProbe();

        QAction *a1 = new QAction(QStringLiteral("Action 1"), this);
        QTest::qWait(1); // event loop re-entry

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
        auto model = new ClientActionModel(this);
        model->setSourceModel(sourceModel);
        QVERIFY(model);
        QCOMPARE(model->rowCount(), 1);

        QSignalSpy changeSpy(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
        QVERIFY(changeSpy.isValid());

        a1->setCheckable(true);
        QCOMPARE(changeSpy.size(), 1);
        a1->setText("Renamed Action");
        QCOMPARE(changeSpy.size(), 2);

        a1->setChecked(true);
        QCOMPARE(changeSpy.size(), 3);

        a1->setDisabled(true);
        QCOMPARE(model->index(0,0).data(Qt::CheckStateRole).toInt(), (int)Qt::Unchecked);
        a1->setEnabled(true);
        QCOMPARE(model->index(0,0).data(Qt::CheckStateRole).toInt(), (int)Qt::Checked);
        QVERIFY(changeSpy.size() >= 4);
        changeSpy.clear();

        QVERIFY(model->setData(model->index(0, 0), Qt::Unchecked, Qt::CheckStateRole));
        QCOMPARE(a1->isEnabled(), false);
        QCOMPARE(changeSpy.size(), 1);

        a1->setChecked(true);
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(model->index(0, 3).data(Qt::CheckStateRole).toInt(), (int)Qt::Checked);
        QVERIFY(model->setData(model->index(0, 3), Qt::Unchecked, Qt::CheckStateRole));
        QCOMPARE(a1->isChecked(), false);
        QCOMPARE(changeSpy.size(), 2);
    }

};

QTEST_MAIN(ActionTest)

#include "actiontest.moc"
