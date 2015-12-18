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

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/paths.h>
#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QtTest/qtest.h>
#include <QObject>
#include <QLibraryInfo>
#include <QTranslator>

using namespace GammaRay;

class TranslatorTest : public QObject
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
    void testCreateDestroy()
    {
        createProbe();

        auto t1 = new QTranslator;
        t1->setObjectName(QStringLiteral("t1"));
        t1->load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + QStringLiteral("/qt_sv.qm"));
        QVERIFY(!t1->isEmpty());
        QCoreApplication::installTranslator(t1);
        QTest::qWait(1);

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslatorsModel"));
        QVERIFY(model);
        ModelTest modelTest(model);
        QCOMPARE(model->rowCount(), 2);

        auto t2 = new QTranslator;
        t2->setObjectName(QStringLiteral("t2"));
        t2->load(QStringLiteral("I_DONT_EXIST_de.qm"));
        QVERIFY(t2->isEmpty());
        QCoreApplication::installTranslator(t2);
        QTest::qWait(1);
        QCOMPARE(model->rowCount(), 2); // empty will not get language change events, so we don't notice it yet, FIXME?
        QCoreApplication::translate("my context", "translate me 1");
        QTest::qWait(1);

        QCoreApplication::removeTranslator(t1);
        delete t1;
        QTest::qWait(1);

        QCOMPARE(model->rowCount(), 2); // see above, we now noticed the empty translator
        QTest::qWait(1);

        QCoreApplication::removeTranslator(t2);
        delete t2;
        QTest::qWait(1);
    }
};

QTEST_MAIN(TranslatorTest)

#include "translatortest.moc"
