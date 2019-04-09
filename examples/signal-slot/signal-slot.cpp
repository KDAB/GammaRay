/*
  signal-slot.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QLCDNumber>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr) :
        QWidget(parent)
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
