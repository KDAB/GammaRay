/*
  propertywidgettest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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
#include <QTreeView>
#include <QGraphicsItem>

#include <core/propertywidget.h>

using namespace GammaRay;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  PropertyWidget widget;
  //widget.setMetaObject((new QTreeView())->metaObject());
  widget.setObject(new QTreeView());
  //widget.setObject(new QGraphicsRectItem, "QGraphicsRectItem");
  widget.show();

  return app.exec();
}
