/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <hooking/hooks.h>
#include <hooking/probecreator.h>
#include <core/probe.h>
#include <common/paths.h>
#include <common/objectbroker.h>

#include <QtTest/qtest.h>

#include <QAbstractItemModel>
#include <QAction>
#include <QObject>

using namespace GammaRay;

class ActionTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::currentProbePath().toUtf8());
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::CreateOnly);
        QTest::qWait(1); // event loop re-entry
    }

private slots:
    void testActionCreationDeletion()
    {
        createProbe();

        QAction *a1 = new QAction("Action 1", this);
        QAction *a2 = new QAction("Action 2", this);
        QTest::qWait(1); // event loop re-entry

        auto *model = ObjectBroker::model("com.kdab.GammaRay.ActionModel");
        QVERIFY(model);
        QCOMPARE(model->rowCount(), 2);

        delete a1;
        QTest::qWait(1); // event loop re-entry

        delete a2;
        QTest::qWait(1); // event loop re-entry
    }

    void testConflictDetection()
    {
      createProbe();

      QAction *a1 = new QAction("Action 1", this);
      a1->setShortcut(QKeySequence("Ctrl+K"));
      QAction *a2 = new QAction("Action 2", this);
      a2->setShortcut(QKeySequence("Ctrl+K"));
      QTest::qWait(1); // event loop re-entry

      auto *model = ObjectBroker::model("com.kdab.GammaRay.ActionModel");
      QVERIFY(model);
      QCOMPARE(model->rowCount(), 2);

      const auto index = model->index(0, 5);
      QCOMPARE(index.data(Qt::DisplayRole).toString(), QKeySequence("Ctrl+K").toString(QKeySequence::NativeText));
    }

};

QTEST_MAIN(ActionTest)

#include "actiontest.moc"
