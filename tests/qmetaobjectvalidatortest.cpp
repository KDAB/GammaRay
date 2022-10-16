/*
  qmetaobjectvalidatortest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <core/qmetaobjectvalidator.cpp>

#include <QDebug>
#include <QTest>

using namespace GammaRay;

struct UnknownCustomType
{
};
struct KnownCustomType
{
};

Q_DECLARE_METATYPE(KnownCustomType)

class QMetaObjectValidatorTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UnknownCustomType failUnknownType READ failUnknownType)
    Q_PROPERTY(KnownCustomType knownType READ knownType)
public:
    static UnknownCustomType failUnknownType()
    {
        return {};
    }
    static KnownCustomType knownType()
    {
        return {};
    }

signals:
    void destroyed();

public slots:
    void unknownParameter(int, UnknownCustomType, int)
    {
    }
    void knownParameter(KnownCustomType)
    {
    }

private slots:
    static void testSignalOverride()
    {
        for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); ++i) {
            const auto method = staticMetaObject.method(i);
            if (method.methodType() == QMetaMethod::Signal)
                QCOMPARE(QMetaObjectValidator::checkMethod(&staticMetaObject, method), QMetaObjectValidatorResult::SignalOverride);
            else
                QVERIFY((QMetaObjectValidator::checkMethod(&staticMetaObject, method) & QMetaObjectValidatorResult::SignalOverride) == 0);
        }
        for (int i = QObject::staticMetaObject.methodOffset(); i < QObject::staticMetaObject.methodCount(); ++i) {
            const auto method = QObject::staticMetaObject.method(i);
            QVERIFY((QMetaObjectValidator::checkMethod(&QObject::staticMetaObject, method) & QMetaObjectValidatorResult::SignalOverride) == 0);
        }
    }

    static void testParameterTypes()
    {
        for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); ++i) {
            const auto method = staticMetaObject.method(i);
            if (method.name().startsWith("unknown")) // krazy:exclude=strings
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                QVERIFY(QMetaObjectValidator::checkMethod(&staticMetaObject, method) == QMetaObjectValidatorResult::NoIssue);
#else
                QVERIFY(QMetaObjectValidator::checkMethod(&staticMetaObject, method) & QMetaObjectValidatorResult::UnknownMethodParameterType);
#endif
            else
                QVERIFY((QMetaObjectValidator::checkMethod(&staticMetaObject, method) & QMetaObjectValidatorResult::UnknownMethodParameterType) == 0);
        }
    }

    static void testPropertyType()
    {
        for (int i = staticMetaObject.propertyOffset(); i < staticMetaObject.propertyCount(); ++i) {
            const auto property = staticMetaObject.property(i);
            if (strstr(property.name(), "fail") == property.name())
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                QVERIFY(QMetaObjectValidator::checkProperty(&staticMetaObject, property) == QMetaObjectValidatorResult::NoIssue);
#else
                QVERIFY(QMetaObjectValidator::checkProperty(&staticMetaObject, property) & QMetaObjectValidatorResult::UnknownPropertyType);
#endif
            else
                QVERIFY((QMetaObjectValidator::checkProperty(&staticMetaObject, property) & QMetaObjectValidatorResult::UnknownPropertyType) == 0);
        }
    }

    static void testObject()
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QCOMPARE(QMetaObjectValidator::check(&staticMetaObject),
                 QMetaObjectValidatorResult::SignalOverride);
#else
        QCOMPARE(QMetaObjectValidator::check(&staticMetaObject),
                 QMetaObjectValidatorResult::SignalOverride | QMetaObjectValidatorResult::UnknownMethodParameterType | QMetaObjectValidatorResult::UnknownPropertyType);
#endif
        QCOMPARE(QMetaObjectValidator::check(&QObject::staticMetaObject), QMetaObjectValidatorResult::NoIssue);
    }
};

QTEST_MAIN(QMetaObjectValidatorTest)

#include "qmetaobjectvalidatortest.moc"
