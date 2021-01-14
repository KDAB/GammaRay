/*
  propertywidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <compat/qasconst.h>

#include <QCoreApplication>
#include <QTimer>

#include <algorithm>

using namespace GammaRay;

QVector<PropertyWidgetTabFactoryBase *> PropertyWidget::s_tabFactories
    = QVector<PropertyWidgetTabFactoryBase *>();
QVector<PropertyWidget *> PropertyWidget::s_propertyWidgets;

PropertyWidget::PropertyWidget(QWidget *parent)
    : QTabWidget(parent)
    , m_tabsUpdatedTimer(new QTimer(this))
    , m_lastManuallySelectedWidget(nullptr)
    , m_controller(nullptr)
{
    m_tabsUpdatedTimer->setInterval(100);
    m_tabsUpdatedTimer->setSingleShot(true);
    s_propertyWidgets.push_back(this);
    connect(this, &QTabWidget::currentChanged, this, &PropertyWidget::slotCurrentTabChanged);
    connect(m_tabsUpdatedTimer, &QTimer::timeout, this, &PropertyWidget::tabsUpdated);
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

    if (Endpoint::instance()->objectAddress(baseName + ".controller")
        == Protocol::InvalidObjectAddress)
        return; // unknown property controller, likely disabled/not supported on the server

    if (m_controller)
        disconnect(m_controller, &PropertyControllerInterface::availableExtensionsChanged, this,
                   &PropertyWidget::updateShownTabs);
    m_controller = ObjectBroker::object<PropertyControllerInterface *>(
        m_objectBaseName + ".controller");
    connect(m_controller, &PropertyControllerInterface::availableExtensionsChanged, this, &PropertyWidget::updateShownTabs);

    updateShownTabs();
}

static void propertyWidgetCleanup()
{
    PropertyWidget::cleanupTabs();
}

void PropertyWidget::registerTab(PropertyWidgetTabFactoryBase *factory)
{
    if (s_tabFactories.isEmpty())
        qAddPostRoutine(propertyWidgetCleanup);
    s_tabFactories.push_back(factory);
    for (PropertyWidget *widget : qAsConst(s_propertyWidgets))
        widget->updateShownTabs();
}

void PropertyWidget::cleanupTabs()
{
    qDeleteAll(s_tabFactories);
}

void PropertyWidget::createWidgets()
{
    if (m_objectBaseName.isEmpty())
        return;
    for (PropertyWidgetTabFactoryBase *factory : qAsConst(s_tabFactories)) {
        if (!factoryInUse(factory) && extensionAvailable(factory)) {
            const PageInfo pi = { factory, factory->createWidget(this) };
            m_pages.push_back(pi);
        }
    }

    std::sort(m_pages.begin(), m_pages.end(), [](const PageInfo &lhs, const PageInfo &rhs) -> bool {
        if (lhs.factory->priority() == rhs.factory->priority())
            return s_tabFactories.indexOf(lhs.factory) < s_tabFactories.indexOf(rhs.factory);
        return lhs.factory->priority() < rhs.factory->priority();
    });
}

void PropertyWidget::updateShownTabs()
{
    setUpdatesEnabled(false);
    createWidgets();

    // we distinguish between the last selected tab, and the last one that
    // was explicitly selected. The latter might be temporarily hidden, but
    // we will try to restore it when it becomes available again.
    auto prevManuallySelected = m_lastManuallySelectedWidget;
    auto prevSelectedWidget = currentWidget();

    int tabIt = 0;
    for (const auto &page : qAsConst(m_pages)) {
        const int index = indexOf(page.widget);
        if (extensionAvailable(page.factory)) {
            if (index != tabIt)
                removeTab(index);
            insertTab(tabIt, page.widget, page.factory->label());
            ++tabIt;
        } else if (index != -1) {
            removeTab(index);
        }
    }

    // try to restore selection
    if (!prevSelectedWidget) // first time
        setCurrentIndex(0);
    else if (indexOf(prevManuallySelected) >= 0)
        setCurrentWidget(prevManuallySelected);
    else if (indexOf(prevSelectedWidget) >= 0)
        setCurrentWidget(prevSelectedWidget);

    // reset to last user selection as this possibly
    // changed as a result of the reording above
    m_lastManuallySelectedWidget = prevManuallySelected;
    setUpdatesEnabled(true);
    m_tabsUpdatedTimer->start(); // use a timer to group chained registrations.
}

bool PropertyWidget::extensionAvailable(PropertyWidgetTabFactoryBase *factory) const
{
    return m_controller->availableExtensions().contains(m_objectBaseName + '.' + factory->name());
}

bool PropertyWidget::factoryInUse(PropertyWidgetTabFactoryBase *factory) const
{
    return std::find_if(m_pages.begin(), m_pages.end(), [factory](const PageInfo &pi) {
        return pi.factory == factory;
    }) != m_pages.end();
}

void PropertyWidget::slotCurrentTabChanged()
{
    m_lastManuallySelectedWidget = currentWidget();
}
