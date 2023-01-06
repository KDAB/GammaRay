/*
  objectreparenttest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
