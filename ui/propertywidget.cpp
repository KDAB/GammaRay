/*
  propertywidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertywidget.h"

#include "common/endpoint.h"
#include "common/objectbroker.h"
#include "common/propertycontrollerinterface.h"

using namespace GammaRay;

QVector<PropertyWidgetTabFactoryBase*> PropertyWidget::s_tabFactories = QVector<PropertyWidgetTabFactoryBase*>();

PropertyWidget::PropertyWidget(QWidget *parent)
  : QTabWidget(parent),
    m_controller(0)
{
  foreach (PropertyWidgetTabFactoryBase *factory, s_tabFactories) {
    QWidget *widget = factory->createWidget(this);
    m_tabWidgets.append(widget);
    addTab(widget, factory->label());
  }
}

PropertyWidget::~PropertyWidget()
{
}

void PropertyWidget::setObjectBaseName(const QString &baseName)
{
  m_objectBaseName = baseName;

  if (Endpoint::instance()->objectAddress(baseName + ".controller") == Protocol::InvalidObjectAddress)
    return; // unknown property controller, likely disabled/not supported on the server

  emit objectBaseNameChanged(baseName);

  if (m_controller) {
    disconnect(m_controller,
               SIGNAL(availableExtensionsChanged(QStringList)),
               this, SLOT(updateShownTabs(QStringList)));
  }
  m_controller =
    ObjectBroker::object<PropertyControllerInterface*>(m_objectBaseName + ".controller");
  connect(m_controller, SIGNAL(availableExtensionsChanged(QStringList)),
          this, SLOT(updateShownTabs(QStringList)));
}

void PropertyWidget::updateShownTabs(QStringList availableExtensions)
{
  setUpdatesEnabled(false);

  for (int i = 0; i < m_tabWidgets.count(); i++) {
    QWidget *widget = m_tabWidgets[i];
    int index = indexOf(widget);
    if (availableExtensions.contains(m_objectBaseName + '.' + s_tabFactories.at(i)->name())) {
      if (index == -1)
        addTab(widget, s_tabFactories.at(i)->label());
    } else if (index != -1) {
      removeTab(index);
    }
  }

  setUpdatesEnabled(true);
}
