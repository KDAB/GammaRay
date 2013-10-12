/*
  propertyeditorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertyeditorfactory.h"
#include "propertycoloreditor.h"
#include "propertyfonteditor.h"
#include "propertyintpaireditor.h"
#include "propertydoublepaireditor.h"
#include "propertypaletteeditor.h"

#include <QItemEditorFactory>

using namespace GammaRay;

PropertyEditorFactory::PropertyEditorFactory()
{
  registerEditor(QVariant::Color, new QStandardItemEditorCreator<PropertyColorEditor>());
  registerEditor(QVariant::Font, new QStandardItemEditorCreator<PropertyFontEditor>());
  registerEditor(QVariant::Palette, new QStandardItemEditorCreator<PropertyPaletteEditor>());
  registerEditor(QVariant::Point, new QStandardItemEditorCreator<PropertyPointEditor>());
  registerEditor(QVariant::PointF, new QStandardItemEditorCreator<PropertyPointFEditor>());
  registerEditor(QVariant::Size, new QStandardItemEditorCreator<PropertySizeEditor>());
  registerEditor(QVariant::SizeF, new QStandardItemEditorCreator<PropertySizeFEditor>());
}

PropertyEditorFactory* PropertyEditorFactory::instance()
{
  static PropertyEditorFactory *s_instance = new PropertyEditorFactory;
  return s_instance;
}


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
QWidget *PropertyEditorFactory::createEditor(QVariant::Type type, QWidget *parent) const
#else
QWidget *PropertyEditorFactory::createEditor(int type, QWidget *parent) const
#endif
{
  QWidget *w = QItemEditorFactory::createEditor(type, parent);
  if (!w) {
    return 0;
  }

  // the read-only view is still in the background usually, so transparency is not a good choice here
  w->setAutoFillBackground(true);
  return w;
}
