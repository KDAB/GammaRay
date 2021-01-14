/*
  timer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QTimerEvent>
#include <QWidget>

class MyUpdateTimer : public QObject
{
    Q_OBJECT
public:
    explicit MyUpdateTimer(QWidget *widget, QObject *parent = nullptr)
        : QObject(parent)
        , m_widget(widget)
    {
    }

    void start()
    {
        startTimer(0);
    }

protected:
    //! [Missing killTimer]
    void timerEvent(QTimerEvent *event) override
    {
        Q_UNUSED(event);
        m_widget->repaint();
        // killTimer(event->timerId());
    }
    //! [Missing killTimer]

private:
    QWidget *m_widget;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget w;
    auto layout = new QVBoxLayout(&w);

    auto btn = new QPushButton("Create QTimer", &w);
    QObject::connect(btn, &QPushButton::clicked, &w, [&w]() {
        //! [Missing setSingleShot]
        auto timer = new QTimer(&w);
        QObject::connect(timer, SIGNAL(timeout()), &w, SLOT(repaint()));
        timer->setInterval(0);
        // timer->setSingleShot(true);
        timer->start();
        //! [Missing setSingleShot]
    });
    layout->addWidget(btn);

    btn = new QPushButton("Start Timer", &w);
    QObject::connect(btn, &QPushButton::clicked, &w, [&w]() {
        auto timer = new MyUpdateTimer(&w, &w);
        timer->start();
    });
    layout->addWidget(btn);

    w.show();
    return app.exec();
}

#include "timer.moc"
