/*
  quickwidgetsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickwidgetsupport.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>

#include <common/metatypedeclarations.h>

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

using namespace GammaRay;

static QuickWidgetSupport *s_quickWidgetSupportInstance = nullptr;

QuickWidgetSupport::QuickWidgetSupport(Probe *probe, QObject *parent)
    : QObject(parent)
    , m_probe(probe)
{
    Q_ASSERT(s_quickWidgetSupportInstance == nullptr);
    s_quickWidgetSupportInstance = this;

    connect(probe, &Probe::objectCreated, this, &QuickWidgetSupport::objectAdded);

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QQuickWidget, QWidget);
    MO_ADD_PROPERTY_RO(QQuickWidget, engine);
    MO_ADD_PROPERTY_RO(QQuickWidget, format);
    MO_ADD_PROPERTY_RO(QQuickWidget, initialSize);
    MO_ADD_PROPERTY_RO(QQuickWidget, quickWindow);
    MO_ADD_PROPERTY_RO(QQuickWidget, rootContext);
    MO_ADD_PROPERTY_RO(QQuickWidget, rootObject);
}

GammaRay::QuickWidgetSupport::~QuickWidgetSupport()
{
    s_quickWidgetSupportInstance = nullptr;
}

void GammaRay::QuickWidgetSupport::objectAdded(QObject *obj)
{
    auto qqw = qobject_cast<QQuickWidget *>(obj);
    if (!qqw)
        return;
    if (m_probe->needsObjectDiscovery())
        m_probe->discoverObject(qqw->quickWindow());
}
