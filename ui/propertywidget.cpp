/*
  propertywidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertywidget.h"

#include "common/endpoint.h"
#include "common/objectbroker.h"
#include "common/propertycontrollerinterface.h"

using namespace GammaRay;

QVector<PropertyWidgetTabFactoryBase*> PropertyWidget::s_tabFactories = QVector<PropertyWidgetTabFactoryBase*>();
QVector<PropertyWidget*> PropertyWidget::s_propertyWidgets;

PropertyWidget::PropertyWidget(QWidget *parent)
  : QTabWidget(parent),
    m_controller(0)
{
  s_propertyWidgets.push_back(this);
}

PropertyWidget::~PropertyWidget()
{
  const int index = s_propertyWidgets.indexOf(this);
  if (index >= 0)
    s_propertyWidgets.remove(index);
}

QString PropertyWidget::objectBaseName() const
{
  Q_ASSERT(!m_objectBaseName.isEmpty());
  return m_objectBaseName;
}

void PropertyWidget::setObjectBaseName(const QString &baseName)
{
  Q_ASSERT(m_objectBaseName.isEmpty()); // ideally the object base name would be a ctor argument, but then this doesn't work in Designer anymore
  m_objectBaseName = baseName;

  if (Endpoint::instance()->objectAddress(baseName + ".controller") == Protocol::InvalidObjectAddress)
    return; // unknown property controller, likely disabled/not supported on the server

  if (m_controller) {
    disconnect(m_controller, SIGNAL(availableExtensionsChanged()), this, SLOT(updateShownTabs()));
  }
  m_controller = ObjectBroker::object<PropertyControllerInterface*>(m_objectBaseName + ".controller");
  connect(m_controller, SIGNAL(availableExtensionsChanged()), this, SLOT(updateShownTabs()));

  updateShownTabs();
}

void PropertyWidget::createWidgets()
{
  if (m_objectBaseName.isEmpty())
    return;
  foreach (PropertyWidgetTabFactoryBase *factory, s_tabFactories) {
    if (!m_usedFactories.contains(factory) && extensionAvailable(factory)) {
      QWidget *widget = factory->createWidget(this);
      m_usedFactories.push_back(factory);
      m_tabWidgets.push_back(widget);
      addTab(widget, factory->label());
    }
  }
}

void PropertyWidget::updateShownTabs()
{
  setUpdatesEnabled(false);
  createWidgets();

  Q_ASSERT(m_tabWidgets.size() == m_usedFactories.size());
  for (int i = 0; i < m_tabWidgets.size(); ++i) {
    QWidget *widget = m_tabWidgets.at(i);
    const int index = indexOf(widget);
    auto factory = m_usedFactories.at(i);
    if (extensionAvailable(factory)) {
      if (index == -1)
        addTab(widget, factory->label());
    } else if (index != -1) {
      removeTab(index);
    }
  }

  setUpdatesEnabled(true);
}

bool PropertyWidget::extensionAvailable(PropertyWidgetTabFactoryBase* factory) const
{
  return m_controller->availableExtensions().contains(m_objectBaseName + '.' + factory->name());
}
