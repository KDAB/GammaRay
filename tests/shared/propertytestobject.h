/*
  propertytestobject.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef PROPERTYTESTOBJECT_H
#define PROPERTYTESTOBJECT_H

#include "changingpropertyobject.h"
#include "variantpropertyobject.h"

#include <QMetaType>

// clazy:excludeall=qproperty-without-notify

class Gadget
{
    Q_GADGET
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 RESET resetProp1)

public:
    Gadget() = default;
    int prop1() const
    {
        return m_prop1;
    }
    void setProp1(int v)
    {
        m_prop1 = v;
    }
    void resetProp1()
    {
        m_prop1 = 5;
    }
    Q_INVOKABLE static void someMethod();

private:
    int m_prop1 = 42;
};

Q_DECLARE_METATYPE(Gadget)
Q_DECLARE_METATYPE(Gadget *)

class PropertyTestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int intProp READ intProp WRITE setIntProp NOTIFY intPropChanged)
    Q_PROPERTY(int readOnlyProp READ intProp RESET resetIntProp)
    Q_PROPERTY(Gadget gadget READ gadget WRITE setGadget)
    Q_PROPERTY(Gadget gadgetReadOnly READ gadget)
    Q_PROPERTY(Gadget *gadgetPointer READ gadgetPointer)
    Q_PROPERTY(VariantPropertyObject *variantPropertyObject READ variantPropertyObject)
    Q_PROPERTY(ChangingPropertyObject *changingPropertyObject READ changingPropertyObject)
public:
    explicit PropertyTestObject(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    int intProp()
    {
        return p1;
    }
    void setIntProp(int i)
    {
        if (p1 == i)
            return;
        p1 = i;
        emit intPropChanged();
    }

    void resetIntProp()
    {
        setIntProp(5);
    }

    Gadget gadget() const
    {
        return g;
    }
    void setGadget(Gadget _g)
    {
        g = _g;
    }
    Gadget *gadgetPointer() const
    {
        return const_cast<Gadget *>(&g);
    }
    VariantPropertyObject *variantPropertyObject()
    {
        return &vpo;
    }
    ChangingPropertyObject *changingPropertyObject()
    {
        return &cpo;
    }

signals:
    void intPropChanged();

private:
    int p1 = 0;
    Gadget g;
    VariantPropertyObject vpo;
    ChangingPropertyObject cpo;
};

#endif // PROPERTYTESTOBJECT_H
