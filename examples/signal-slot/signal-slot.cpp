/*
  signal-slot.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QApplication>
#include <QLCDNumber>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        //! [Widget setup]
        auto layout = new QVBoxLayout(this);
        m_emitButton = new QPushButton("Emit Signal", this);
        layout->addWidget(m_emitButton);
        connect(m_emitButton, &QPushButton::clicked, this, &Widget::signalEmitted);

        auto reconnectButton = new QPushButton("Reconnect Signal", this);
        layout->addWidget(reconnectButton);
        connect(reconnectButton, &QPushButton::clicked, this, &Widget::reconnectSignal);

        m_counter = new QLCDNumber(this);
        layout->addWidget(m_counter);
        //! [Widget setup]
    }

public slots:
    void reconnectSignal()
    {
        //! [Mismatching reconnect]
        disconnect(m_emitButton, &QPushButton::clicked, this, &Widget::signalEmitted);
        connect(m_emitButton, &QAbstractButton::clicked, this, &Widget::signalEmitted);
        //! [Mismatching reconnect]
    }

    void signalEmitted()
    {
        m_counter->display(m_counter->intValue() + 1);
    }

private:
    QPushButton *m_emitButton;
    QLCDNumber *m_counter;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Widget w;
    w.show();
    return app.exec();
}

#include "signal-slot.moc"
