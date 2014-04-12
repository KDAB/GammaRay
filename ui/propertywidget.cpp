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
    disconnect(m_controller,
               SIGNAL(availableExtensionsChanged(QStringList)),
               this, SLOT(updateShownTabs(QStringList)));
  }
  m_controller =
    ObjectBroker::object<PropertyControllerInterface*>(m_objectBaseName + ".controller");
  connect(m_controller, SIGNAL(availableExtensionsChanged(QStringList)),
          this, SLOT(updateShownTabs(QStringList)));

  createWidgets();
}

void PropertyWidget::createWidgets()
{
  if (m_objectBaseName.isEmpty())
    return;
  foreach (PropertyWidgetTabFactoryBase *factory, s_tabFactories) {
    if (!m_tabWidgets.contains(factory)) {
      QWidget *widget = factory->createWidget(this);
      m_tabWidgets.insert(factory, widget);
      addTab(widget, factory->label());
    }
  }
}

void PropertyWidget::updateShownTabs(const QStringList &availableExtensions)
{
  setUpdatesEnabled(false);

  for (QHash<PropertyWidgetTabFactoryBase*, QWidget*>::const_iterator it = m_tabWidgets.constBegin(); it != m_tabWidgets.constEnd(); ++it) {
    QWidget *widget = it.value();
    const int index = indexOf(widget);
    if (availableExtensions.contains(m_objectBaseName + '.' + it.key()->name())) {
      if (index == -1)
        addTab(widget, it.key()->label());
    } else if (index != -1) {
      removeTab(index);
    }
  }

  setUpdatesEnabled(true);
}
