/*
  quickwidgetsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <common/objectbroker.h>

#include <QDebug>
#include <QMetaObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

using namespace GammaRay;

static QuickWidgetSupport *s_quickWidgetSupportInstance = Q_NULLPTR;

static bool quickWidgetGrabWindowCallback(QQuickWindow *window)
{
    if (!s_quickWidgetSupportInstance)
        return false;
    return s_quickWidgetSupportInstance->grabWindow(window);
}

QuickWidgetSupport::QuickWidgetSupport(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
    , m_quickInspector(Q_NULLPTR)
    , m_probe(probe)
{
    Q_ASSERT(s_quickWidgetSupportInstance == Q_NULLPTR);
    s_quickWidgetSupportInstance = this;

    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), this, SLOT(objectAdded(QObject*)));

    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QQuickWidget, QWidget);
    MO_ADD_PROPERTY_RO(QQuickWidget, QQmlEngine *, engine);
    MO_ADD_PROPERTY_RO(QQuickWidget, QSurfaceFormat, format);
    MO_ADD_PROPERTY_RO(QQuickWidget, QSize, initialSize);
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    MO_ADD_PROPERTY_RO(QQuickWidget, QQuickWindow *, quickWindow);
#endif
    MO_ADD_PROPERTY_RO(QQuickWidget, QQmlContext *, rootContext);
    MO_ADD_PROPERTY_RO(QQuickWidget, QQuickItem *, rootObject);
}

GammaRay::QuickWidgetSupport::~QuickWidgetSupport()
{
    s_quickWidgetSupportInstance = Q_NULLPTR;
}

void GammaRay::QuickWidgetSupport::objectAdded(QObject *obj)
{
    auto qqw = qobject_cast<QQuickWidget *>(obj);
    if (!qqw)
        return;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    m_windowMap.insert(qqw->quickWindow(), qqw);
    if (m_probe->needsObjectDiscovery())
        m_probe->discoverObject(qqw->quickWindow());
#endif
    registerWindowGrabber();
}

void GammaRay::QuickWidgetSupport::registerWindowGrabber()
{
    if (m_quickInspector)
        return;

    if (!ObjectBroker::hasObject(QStringLiteral("com.kdab.GammaRay.QuickInspectorInterface/1.0"))) {
        // the QQ2 inspector can be activated after ourselves, so try again
        QMetaObject::invokeMethod(this, "registerWindowGrabber", Qt::QueuedConnection);
        return;
    }

    m_quickInspector
        = ObjectBroker::objectInternal(QStringLiteral(
                                           "com.kdab.GammaRay.QuickInspectorInterface/1.0"));
    Q_ASSERT(m_quickInspector);
    QMetaObject::invokeMethod(m_quickInspector, "registerGrabWindowCallback",
                              Q_ARG(GrabWindowCallback, quickWidgetGrabWindowCallback));
}

bool GammaRay::QuickWidgetSupport::grabWindow(QQuickWindow *window) const
{
    const auto it = m_windowMap.constFind(window);
    if (it == m_windowMap.constEnd())
        return false;

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    auto image = it.value()->grabFramebuffer();
    // See QTBUG-53795
    image.setDevicePixelRatio(it.value()->quickWindow()->effectiveDevicePixelRatio());
    QMetaObject::invokeMethod(m_quickInspector, "sendRenderedScene", Q_ARG(QImage, image));
    return true;
#else
    return false;
#endif
}
