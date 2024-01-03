/*
  widget-layouting.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "ui_contactform.h"

#include <QApplication>

class ContactForm : public QWidget
{
    Q_OBJECT
public:
    explicit ContactForm(QWidget *parent = nullptr)
        : QWidget(parent)
        , ui(new Ui::ContactForm)
    {
        ui->setupUi(this);
    }

private:
    QScopedPointer<Ui::ContactForm> ui;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QWidget w1;
    QWidget w2;
    ContactForm form;
    form.show();
    return app.exec();
}

#include "widget-layouting.moc"
