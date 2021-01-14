/*
  changingpropertyobject.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

    int staticChangingProperty() { return m_count; }
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
