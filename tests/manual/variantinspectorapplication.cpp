/*
  variantinspectorapplication.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "variantinspector.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  VariantInspector vi;

  QHash<QString, int> mapping;
  mapping.insert("One", 1);
  mapping.insert("Two", 2);

  vi.setProperty("dynamicProperty", QVariant::fromValue(mapping));

  return app.exec();
}
