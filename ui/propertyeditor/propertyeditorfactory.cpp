/*
  propertyeditorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
  initBuiltInTypes();

  addEditor(QVariant::Color, new QStandardItemEditorCreator<PropertyColorEditor>());
  addEditor(QVariant::Font, new QStandardItemEditorCreator<PropertyFontEditor>());
  addEditor(QVariant::Palette, new QStandardItemEditorCreator<PropertyPaletteEditor>());
  addEditor(QVariant::Point, new QStandardItemEditorCreator<PropertyPointEditor>());
  addEditor(QVariant::PointF, new QStandardItemEditorCreator<PropertyPointFEditor>());
  addEditor(QVariant::Size, new QStandardItemEditorCreator<PropertySizeEditor>());
  addEditor(QVariant::SizeF, new QStandardItemEditorCreator<PropertySizeFEditor>());
}

PropertyEditorFactory* PropertyEditorFactory::instance()
{
  static PropertyEditorFactory *s_instance = new PropertyEditorFactory;
  return s_instance;
}


QWidget *PropertyEditorFactory::createEditor(TypeId type, QWidget *parent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  if (type == QMetaType::Float)
    type = QVariant::Double;
#endif

  QWidget *w = QItemEditorFactory::createEditor(type, parent);
  if (!w) {
    return 0;
  }

  // the read-only view is still in the background usually, so transparency is not a good choice here
  w->setAutoFillBackground(true);
  return w;
}

QVector< int > PropertyEditorFactory::supportedTypes()
{
  return instance()->m_supportedTypes;
}

void PropertyEditorFactory::initBuiltInTypes()
{
  m_supportedTypes
    << QVariant::Bool
    << QVariant::Double
    << QVariant::Int
    << QVariant::UInt
    << QVariant::Date
    << QVariant::DateTime
    << QVariant::String
    << QVariant::Time;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_supportedTypes << QMetaType::Float;
#endif
}

void PropertyEditorFactory::addEditor(PropertyEditorFactory::TypeId type, QItemEditorCreatorBase* creator)
{
  registerEditor(type, creator);
  m_supportedTypes.push_back(type);
}
