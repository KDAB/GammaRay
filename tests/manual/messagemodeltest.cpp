/*
  messagemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

/*some Windows compilers don't like Q_NORETURN*/
/*error C2381: 'GammaRay::MessageGenerator::generateFatal': redefinition; '__declspec(noreturn)' or '[[noreturn]]' differs*/
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
