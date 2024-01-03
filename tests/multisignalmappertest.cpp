/*
  multisignalmappertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        qRegisterMetaType<QVector<QVariant>>();
    }

private:
    static QMetaMethod method(QObject *obj, const char *name)
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

        QSignalSpy spy(&mapper, &MultiSignalMapper::signalEmitted);
        QVERIFY(spy.isValid());
        QVERIFY(spy.isEmpty());

        emit emitter1.signal1(42);
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).value<QObject *>(), &emitter1);
        QCOMPARE(spy.at(0).at(1).toInt(), emitter1.metaObject()->indexOfSignal("signal1(int)"));
        QCOMPARE(spy.at(0).at(2).value<QVector<QVariant>>().first().toInt(), 42);

        emit emitter2.signal2(QStringLiteral("hello"));
        QCOMPARE(spy.size(), 2);
        QCOMPARE(spy.at(1).at(0).value<QObject *>(), &emitter2);
        QCOMPARE(spy.at(1).at(1).toInt(), emitter1.metaObject()->indexOfSignal("signal2(QString)"));
        QCOMPARE(spy.at(1).at(2).value<QVector<QVariant>>().first().toString(),
                 QStringLiteral("hello"));
    }
};

QTEST_MAIN(MultiSignalMapperTest)

#include "multisignalmappertest.moc"
