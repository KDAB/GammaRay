/*
  styleinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "styleinspector.h"
#include "complexcontrolmodel.h"
#include "controlmodel.h"
#include "palettemodel.h"
#include "pixelmetricmodel.h"
#include "primitivemodel.h"
#include "standardiconmodel.h"
#include "ui_styleinspector.h"

#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"
#include "include/singlecolumnobjectproxymodel.h"

#include <QApplication>

using namespace GammaRay;

StyleInspector::StyleInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::StyleInspector),
    m_primitiveModel(new PrimitiveModel(this)),
    m_controlModel(new ControlModel(this)),
    m_complexControlModel(new ComplexControlModel(this)),
    m_pixelMetricModel(new PixelMetricModel(this)),
    m_standardIconModel(new StandardIconModel(this)),
    m_standardPaletteModel(new PaletteModel(this))
{
  ui->setupUi(this);

  ObjectTypeFilterProxyModel<QStyle> *styleFilter = new ObjectTypeFilterProxyModel<QStyle>(this);
  styleFilter->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(styleFilter);
  ui->styleSelector->setModel(singleColumnProxy);
  connect(ui->styleSelector, SIGNAL(activated(int)), SLOT(styleSelected(int)));

  ui->primitivePage->setModel(m_primitiveModel);
  ui->controlPage->setModel(m_controlModel);
  ui->complexControlPage->setModel(m_complexControlModel);

  ui->pixelMetricView->setModel(m_pixelMetricModel);
  ui->pixelMetricView->header()->setResizeMode(QHeaderView::ResizeToContents);

  ui->standardIconView->setModel(m_standardIconModel);
  ui->standardIconView->header()->setResizeMode(QHeaderView::ResizeToContents);

  ui->standardPaletteView->setModel(m_standardPaletteModel);
  ui->standardIconView->header()->setResizeMode(QHeaderView::ResizeToContents);

  if (ui->styleSelector->count()) {
    styleSelected(0);
  }
}

StyleInspector::~StyleInspector()
{
  delete ui;
}

void StyleInspector::styleSelected(int index)
{
  QObject *obj = ui->styleSelector->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QStyle *style = qobject_cast<QStyle*>(obj);
  m_primitiveModel->setStyle(style);
  m_controlModel->setStyle(style);
  m_complexControlModel->setStyle(style);
  m_pixelMetricModel->setStyle(style);
  m_standardIconModel->setStyle(style);
  m_standardPaletteModel->setPalette(style ? style->standardPalette() : qApp->palette());
}

#include "styleinspector.moc"
