/*
  messagemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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
    qWarning() << "A warning message" << "split into two parts";
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
