/*
  messagemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "messagemodeltest.h"

#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

using namespace GammaRay;

MessageGenerator::MessageGenerator()
{
    auto *layout = new QVBoxLayout;

    QPushButton *button = new QPushButton(QStringLiteral("debug"));
    connect(button, &QAbstractButton::clicked, this, &MessageGenerator::generateDebug);
    layout->addWidget(button);

    button = new QPushButton(QStringLiteral("warning"));
    connect(button, &QAbstractButton::clicked, this, &MessageGenerator::generateWarning);
    layout->addWidget(button);

    button = new QPushButton(QStringLiteral("critical"));
    connect(button, &QAbstractButton::clicked, this, &MessageGenerator::generateCritical);
    layout->addWidget(button);

    button = new QPushButton(QStringLiteral("fatal"));
    connect(button, &QAbstractButton::clicked, this, &MessageGenerator::generateFatal);
    layout->addWidget(button);

    setLayout(layout);
}

void MessageGenerator::generateDebug()
{
    qDebug() << "A debug message";
}

void MessageGenerator::generateWarning()
{
    qWarning() << "A warning message"
               << "split into two parts";
}

void MessageGenerator::generateCritical()
{
    qCritical() << "A critical message";
}

void MessageGenerator::generateFatal()
{
    qFatal("A fatal message");
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MessageGenerator generator;
    generator.show();

    return app.exec();
}
