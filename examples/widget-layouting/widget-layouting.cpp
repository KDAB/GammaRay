/*
  widget-layouting.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "ui_contactform.h"

#include <QApplication>

class ContactForm : public QWidget
{
    Q_OBJECT
public:
    explicit ContactForm(QWidget *parent = nullptr) :
        QWidget(parent),
        ui(new Ui::ContactForm)
    {
        ui->setupUi(this);
    }

private:
    QScopedPointer<Ui::ContactForm> ui;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QWidget w1;
    QWidget w2;
    ContactForm form;
    form.show();
    return app.exec();
}

#include "widget-layouting.moc"
