/*
  complexcontrolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "complexcontrolmodel.h"
#include "styleoption.h"

#include <QPainter>
#include <QStyleOption>

using namespace GammaRay;

struct complex_control_element_t {
    const char *name;
    QStyle::ComplexControl control;
    QStyleOption* (*styleOptionFactory)();
};

#define MAKE_CC1( control ) { #control , QStyle:: control, &StyleOption::makeStyleOptionComplex }
#define MAKE_CC2( control, factory ) { #control, QStyle:: control, &StyleOption:: factory }

static complex_control_element_t complexControlElements[] =  {
  MAKE_CC1(CC_SpinBox),
  MAKE_CC1(CC_ComboBox),
  MAKE_CC1(CC_ScrollBar),
  MAKE_CC1(CC_Slider),
  MAKE_CC1(CC_ToolButton),
  MAKE_CC1(CC_TitleBar),
  MAKE_CC1(CC_Q3ListView),
  MAKE_CC1(CC_Dial),
  MAKE_CC1(CC_GroupBox),
  MAKE_CC1(CC_MdiControls)
};


ComplexControlModel::ComplexControlModel(QObject* parent): AbstractStyleElementStateTable(parent)
{
}

QVariant ComplexControlModel::doData(int row, int column, int role) const
{
  if (role == Qt::DecorationRole) {
    QPixmap pixmap(cellWidth(), cellHeight());
    QPainter painter(&pixmap);
    drawTransparencyBackground(&painter, pixmap.rect());

    QScopedPointer<QStyleOptionComplex> opt(qstyleoption_cast<QStyleOptionComplex*>(complexControlElements[row].styleOptionFactory()));
    Q_ASSERT(opt);
    opt->rect = pixmap.rect();
    opt->palette = m_style->standardPalette();
    opt->state = StyleOption::prettyState(column);
    m_style->drawComplexControl(complexControlElements[row].control, opt.data(), &painter);
    return pixmap;
  }

  return AbstractStyleElementStateTable::doData(row, column, role);
}

int ComplexControlModel::doRowCount() const
{
  return sizeof(complexControlElements) / sizeof(complexControlElements[0]);
}

QVariant ComplexControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    return complexControlElements[section].name;
  return AbstractStyleElementStateTable::headerData(section, orientation, role);
}

#include "complexcontrolmodel.moc"
