/*
  signalspycallbacktest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <QAbstractEventDispatcher>
#include <QEventLoop>
#include <QPointer>
#include <QTimer>
#include <QVector>

#include <algorithm>
#include <utility>

using namespace GammaRay;

class Sender : public QObject
{
    Q_OBJECT
public:
    void emitSignal()
    {
        emit mySignal();
    }

signals:
    void mySignal();
};

class Receiver : public QObject
{
    Q_OBJECT
public slots:
    void senderDeletingSlot()
    {
        delete sender();
    }
};

// the emissions the signal begin callback reported, as caller and method index
static QVector<std::pair<QObject *, int>> s_emissions;

class SignalSpyCallbackTest : public BaseProbeTest
{
    Q_OBJECT
private:
    static bool hasEmission(QObject *caller)
    {
        return std::any_of(s_emissions.cbegin(), s_emissions.cend(), [caller](const std::pair<QObject *, int> &emission) {
            return emission.first == caller;
        });
    }

private slots:
    void testSenderDeletion()
    {
        createProbe();

        QPointer<Sender> s1 = new Sender;
        QPointer<Sender> s2 = new Sender;
        Receiver r;

        connect(s1.data(), &Sender::mySignal, &r, &Receiver::senderDeletingSlot);
        s1->emitSignal(); // must not crash
        QVERIFY(s1.isNull());

        // give the probe time to process s and r2 (needs one event loop re-entry)
        QTest::qWait(1);

        connect(s2.data(), &Sender::mySignal, &r, &Receiver::senderDeletingSlot);
        s2->emitSignal(); // must not crash
        QVERIFY(s2.isNull());

        delete s1.data();
        delete s2.data();
    } // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

    // the callback sees the application's signals, but not every one of them
    void testSignalBeginCallback()
    {
        if (!Probe::instance())
            createProbe();
        SignalSpyCallbackSet callbacks;
        callbacks.signalBeginCallback = [](QObject *caller, int methodIndex, void **) {
            s_emissions.push_back({ caller, methodIndex });
        };
        Probe::instance()->registerSignalSpyCallbackSet(callbacks);

        Sender sender;
        s_emissions.clear();
        sender.emitSignal();
        const QVector<std::pair<QObject *, int>> expected { { &sender, Sender::staticMetaObject.indexOfSignal("mySignal()") } };
        QCOMPARE(s_emissions, expected);

        // not destroyed(), which Qt only emits when something is connected to it
        auto obj = new QObject;
        bool destroyed = false;
        connect(obj, &QObject::destroyed, this, [&destroyed] { destroyed = true; });
        s_emissions.clear();
        delete obj;
        QVERIFY(destroyed);
        QVERIFY(!hasEmission(obj));

        // not the event dispatcher's signals, it emits these when the event loop waits
        auto dispatcher = QAbstractEventDispatcher::instance();
        QVERIFY(dispatcher);
        int aboutToBlock = 0;
        const auto connection = connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock, this, [&aboutToBlock] { ++aboutToBlock; });
        s_emissions.clear();
        QEventLoop loop; // QTest::qWait() doesn't block in the event dispatcher
        QTimer::singleShot(10, &loop, &QEventLoop::quit);
        loop.exec();
        disconnect(connection);
        QVERIFY(aboutToBlock > 0);
        QVERIFY(!hasEmission(dispatcher));

        // not the signals of GammaRay's own objects
        s_emissions.clear();
        Probe::instance()->selectObject(&sender);
        QVERIFY(!hasEmission(Probe::instance()));
    }

    static void cleanupTestCase()
    {
        // explicitly delete the probe as our usual cleanup doesn't work since we will
        // not get qApp::aboutToQuit() from QTest::qExec(), and then we end up with
        // still alive QSFPM when the static deleter kills the source model null object
        // used by them internally, which then crashes with Qt4...
        delete Probe::instance();
    }
};

QTEST_MAIN(SignalSpyCallbackTest)

#include "signalspycallbacktest.moc"
