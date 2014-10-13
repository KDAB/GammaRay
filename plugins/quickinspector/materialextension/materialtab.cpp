/*
  materialtab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "materialtab.h"
#include "materialextensioninterface.h"
#include "ui_materialtab.h"
#include <ui/propertywidget.h>

#include "common/objectbroker.h"

using namespace GammaRay;

MaterialTab::MaterialTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_MaterialTab),
  m_interface(0)
{
  m_ui->setupUi(this);
  setObjectBaseName(parent->objectBaseName());
  connect(m_ui->shaderList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(shaderSelectionChanged(QItemSelection)));

  m_ui->splitter->setStretchFactor(0, 1);
  m_ui->splitter->setStretchFactor(1, 3);
}

MaterialTab::~MaterialTab()
{
}

void MaterialTab::setObjectBaseName(const QString &baseName)
{
  if (m_interface) {
    disconnect(m_interface, 0, this, 0);
  }

  m_interface =
    ObjectBroker::object<MaterialExtensionInterface*>(baseName + ".material");
  connect(m_interface, SIGNAL(gotShader(QString)), this, SLOT(showShader(QString)));

  m_ui->materialPropertyView->setModel(ObjectBroker::model(baseName + ".materialPropertyModel"));
  m_ui->shaderList->setModel(ObjectBroker::model(baseName + ".shaderModel"));
}

void MaterialTab::shaderSelectionChanged(const QItemSelection& selection)
{
  m_ui->shaderEdit->clear();
  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();
  if (!index.isValid())
    return;
  m_interface->getShader(index.data(Qt::DisplayRole).toString());
}

void MaterialTab::showShader(const QString &shaderSource)
{
  m_ui->shaderEdit->setText(shaderSource);
}
