/*
  messagemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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
  QVBoxLayout *layout  = new QVBoxLayout;

  QPushButton *button = new QPushButton("debug");
  connect(button, SIGNAL(clicked(bool)), SLOT(generateDebug()));
  layout->addWidget(button);

  button = new QPushButton("warning");
  connect(button, SIGNAL(clicked(bool)), SLOT(generateWarning()));
  layout->addWidget(button);

  button = new QPushButton("critical");
  connect(button, SIGNAL(clicked(bool)), SLOT(generateCritical()));
  layout->addWidget(button);

  button = new QPushButton("fatal");
  connect(button, SIGNAL(clicked(bool)), SLOT(generateFatal()));
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

