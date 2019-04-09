/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QTimer>

class MyObject : public QObject
{
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr)
        : QObject(parent)
        , c(new QObject(this))
        , p1(new QObject(this))
        , p2(new QObject(this))
    {
        c->setObjectName(QStringLiteral("MovingSubtree"));

        auto t = new QTimer(this);
        t->start(10000);
        connect(t, &QTimer::timeout, this, &MyObject::reparent);

        auto gc = new QObject(c);
        new QObject(gc);
        c->setParent(p1);
    }

public slots:
    void reparent()
    {
        if (c->parent() == p1)
            c->setParent(p2);
        else if (c->parent() == p2)
            c->setParent(nullptr);
        else
            c->setParent(p1);
    }

private:
    QObject *c, *p1, *p2;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    MyObject obj;
    obj.setObjectName(QStringLiteral("MyObject"));

    return app.exec();
}

#include "objectreparenttest.moc"
