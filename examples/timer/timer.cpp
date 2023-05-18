/*
  timer.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        QObject::connect(timer, &QTimer::timeout, &w, [&w] { w.repaint(); });
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
