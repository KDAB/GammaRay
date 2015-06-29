/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QCoreApplication>
#include <QDebug>
#include <QMetaType>

class MyGadget
{
    Q_GADGET
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1)

public:
    inline MyGadget() : m_prop1(42) {}
    inline int prop1() const { return m_prop1; }
    inline void setProp1(int v) { m_prop1 = v; }

    Q_INVOKABLE void someMethod();

private:
    int m_prop1;
};

void MyGadget::someMethod()
{
    qDebug() << Q_FUNC_INFO;
}

Q_DECLARE_METATYPE(MyGadget)


class MyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MyGadget gadget READ gadget)

public:
    MyGadget gadget() const { return MyGadget(); }
};


int main(int argc, char** argv)
{
    qRegisterMetaType<MyGadget>();
    QCoreApplication app(argc, argv);

    MyObject obj;
    obj.setObjectName("MyObject");

    return app.exec();
}

#include "gadgettest.moc"
