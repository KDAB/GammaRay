/*
  changingpropertyobject.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef CHANGINGPROPERTYOBJECT_H
#define CHANGINGPROPERTYOBJECT_H

#include <QObject>
#include <QVariant>

class ChangingPropertyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        int staticChangingProperty READ staticChangingProperty RESET staticChangingPropertyReset NOTIFY staticChangingPropertyChanged)

public:
    explicit ChangingPropertyObject(QObject *parent = nullptr)
        : QObject(parent)
        , m_count(0)
    {
        setObjectName(QStringLiteral("changingPropertyObject"));
    }

    int staticChangingProperty()
    {
        return m_count;
    }
    void staticChangingPropertyReset()
    {
        m_count = 0;
        emit staticChangingPropertyChanged();
    }

signals:
    void staticChangingPropertyChanged();

public slots:
    void changeProperties()
    {
        ++m_count;
        setProperty("dynamicChangingProperty", QVariant(m_count));
        emit staticChangingPropertyChanged();
    }

private:
    int m_count;
};

#endif // CHANGINGPROPERTYOBJECT_H
