/*
  signalspycallbacktest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <QPointer>

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

class SignalSpyCallbackTest : public BaseProbeTest
{
    Q_OBJECT
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
