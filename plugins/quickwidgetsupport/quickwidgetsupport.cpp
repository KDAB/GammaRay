/*
  quickwidgetsupport.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
