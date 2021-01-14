/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "core/multisignalmapper.h"

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QVector>
#include <QTest>

using namespace GammaRay;

class Emitter : public QObject
{
    Q_OBJECT
signals:
    void signal1(int);
    void signal2(const QString &);

    friend class MultiSignalMapperTest;
};

Q_DECLARE_METATYPE(QVector<QVariant>)

class MultiSignalMapperTest : public QObject
{
    Q_OBJECT
public:
    explicit MultiSignalMapperTest(QObject *parent = nullptr)
        : QObject(parent)
    {
        qRegisterMetaType<QVector<QVariant> >();
    }

private:
    QMetaMethod method(QObject *obj, const char *name)
    {
        return obj->metaObject()->method(obj->metaObject()->indexOfSignal(name));
    }

private slots:
    void testMapper()
    {
        Emitter emitter1, emitter2;

        MultiSignalMapper mapper;
        mapper.connectToSignal(&emitter1, method(&emitter1, "signal1(int)"));
        mapper.connectToSignal(&emitter2, method(&emitter1, "signal1(int)"));
        mapper.connectToSignal(&emitter1, method(&emitter1, "signal2(QString)"));
        mapper.connectToSignal(&emitter2, method(&emitter1, "signal2(QString)"));

        QSignalSpy spy(&mapper, SIGNAL(signalEmitted(QObject*,int,QVector<QVariant>)));
        QVERIFY(spy.isValid());
        QVERIFY(spy.isEmpty());

        emit emitter1.signal1(42);
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).value<QObject *>(), &emitter1);
        QCOMPARE(spy.at(0).at(1).toInt(), emitter1.metaObject()->indexOfSignal("signal1(int)"));
        QCOMPARE(spy.at(0).at(2).value<QVector<QVariant> >().first().toInt(), 42);

        emit emitter2.signal2(QStringLiteral("hello"));
        QCOMPARE(spy.size(), 2);
        QCOMPARE(spy.at(1).at(0).value<QObject *>(), &emitter2);
        QCOMPARE(spy.at(1).at(1).toInt(), emitter1.metaObject()->indexOfSignal("signal2(QString)"));
        QCOMPARE(spy.at(1).at(2).value<QVector<QVariant> >().first().toString(),
                 QStringLiteral("hello"));
    }
};

QTEST_MAIN(MultiSignalMapperTest)

#include "multisignalmappertest.moc"
