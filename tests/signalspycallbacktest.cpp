/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <hooking/probecreator.h>

#include <QtTest/qtest.h>
#include <QObject>
#include <QPointer>

using namespace GammaRay;

class Sender : public QObject
{
    Q_OBJECT
public:
    void emitSignal() { emit mySignal(); }

signals:
    void mySignal();
};

class Receiver : public QObject
{
    Q_OBJECT
public slots:
    void senderDeletingSlot() { delete sender(); }
};

class SignalSpyCallbackTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        qputenv("GAMMARAY_ProbePath", QCoreApplication::applicationDirPath().toUtf8());
        new ProbeCreator(ProbeCreator::CreateOnly);
        QTest::qWait(1); // event loop re-entry
    }

private slots:
    void testSenderDeletion()
    {
        createProbe();

        QPointer<Sender> s1 = new Sender;
        QPointer<Sender> s2 = new Sender;
        Receiver r;

        connect(s1, SIGNAL(mySignal()), &r, SLOT(senderDeletingSlot()));
        s1->emitSignal(); // must not crash
        QVERIFY(s1.isNull());

        // give the probe time to process s and r2 (needs one event loop re-entry)
        QTest::qWait(1);

        connect(s2, SIGNAL(mySignal()), &r, SLOT(senderDeletingSlot()));
        s2->emitSignal(); // must not crash
        QVERIFY(s2.isNull());
    }
};

QTEST_MAIN(SignalSpyCallbackTest)

#include "signalspycallbacktest.moc"
