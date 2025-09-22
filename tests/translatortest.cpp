/*
  translatortest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectbroker.h>

#include <QAbstractItemModelTester>
#include <QItemSelectionModel>
#include <QLibraryInfo>
#include <QTranslator>

using namespace GammaRay;

class TranslatorTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    void testCreateDestroy()
    {
        createProbe();

        auto t1 = new QTranslator;
        t1->setObjectName(QStringLiteral("t1"));
        QVERIFY(t1->load(QLibraryInfo::path(QLibraryInfo::TranslationsPath) + QStringLiteral("/qt_pt_PT.qm")));
        QVERIFY(!t1->isEmpty());
        QCoreApplication::installTranslator(t1);
        QTest::qWait(1);

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslatorsModel"));
        QVERIFY(model);
        QAbstractItemModelTester modelTest(model);
        QCOMPARE(model->rowCount(), 2);

        auto t2 = new QTranslator;
        t2->setObjectName(QStringLiteral("t2"));
        QVERIFY(!t2->load(QStringLiteral("I_DONT_EXIST_de.qm")));
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

    void testTranslate()
    {
        createProbe();

        auto t1 = new QTranslator;
        t1->setObjectName(QStringLiteral("t1"));
        QVERIFY(t1->load(QLibraryInfo::path(QLibraryInfo::TranslationsPath) + QStringLiteral("/qt_pt_PT.qm")));
        QVERIFY(!t1->isEmpty());
        QCoreApplication::installTranslator(t1);
        QTest::qWait(1);

        auto *translatorModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslatorsModel"));
        QVERIFY(translatorModel);
        QAbstractItemModelTester TranslationsModelTest(translatorModel);
        QCOMPARE(translatorModel->rowCount(), 2);

        auto translatorSelection = ObjectBroker::selectionModel(translatorModel);
        QVERIFY(translatorSelection);
        translatorSelection->select(translatorModel->index(0, 0), QItemSelectionModel::ClearAndSelect);

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslationsModel"));
        QVERIFY(model);
        QAbstractItemModelTester modelTest(model);

        QCoreApplication::translate("context", "key", nullptr);
        QCoreApplication::translate(nullptr, "key", nullptr);
        QCoreApplication::translate(nullptr, "key", "disambiguation");
        QCoreApplication::translate("context", "key", "disambiguation");

        delete t1;
        QTest::qWait(1);
    }
};

QTEST_MAIN(TranslatorTest)

#include "translatortest.moc"
