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
  connect(m_ui->shaderList, SIGNAL(itemActivated(QListWidgetItem*)),
          this, SLOT(onShaderSelected(QListWidgetItem*)));
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
  connect(m_interface, SIGNAL(shaderListChanged(QStringList)), this, SLOT(setShaders(QStringList)));
  connect(m_interface, SIGNAL(gotShader(QString)), this, SLOT(showShader(QString)));
}

void MaterialTab::setShaders(const QStringList &shaderSources)
{
  m_shaderSources = shaderSources;
  m_ui->shaderEdit->setText("");
  m_ui->shaderList->clear();
  if (shaderSources.size() > 0) {
    foreach (const QString &fileName, shaderSources) {
      new QListWidgetItem(fileName, m_ui->shaderList);
    }
  }
}

void MaterialTab::onShaderSelected(QListWidgetItem *item)
{
  m_interface->getShader(item->text());
}

void MaterialTab::showShader(const QString &shaderSource)
{
  m_ui->shaderEdit->setText(shaderSource);
}
