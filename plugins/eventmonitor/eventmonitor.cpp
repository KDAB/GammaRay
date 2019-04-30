/*
  styleinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tim Henning <tim.henning@kdab.com>

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

#include "eventmonitor.h"

#include "eventmodelroles.h"

#include <core/aggregatedpropertymodel.h>
#include <core/metaobjectrepository.h>
#include <core/metaobject.h>
#include <core/objectinstance.h>
#include <core/remote/serverproxymodel.h>

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QDebug>
#include <QItemSelectionModel>
#include <QMutex>
#include <QSortFilterProxyModel>

using namespace GammaRay;


static EventModel *s_model = nullptr;


QString eventTypeToClassName(QEvent::Type type) {
    switch (type) {
    case QEvent::NonClientAreaMouseMove:
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonDblClick:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return "QMouseEvent";
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
        return "QTouchEvent";
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::TabletEnterProximity:
    case QEvent::TabletLeaveProximity:
        return "QTabletEvent";
    case QEvent::NativeGesture:
        return "QNativeGestureEvent";
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        return "QKeyEvent";
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::FocusAboutToChange:
        return "QFocusEvent";
    case QEvent::Move:
        return "QMoveEvent";
    case QEvent::Paint:
        return "QPaintEvent";
    case QEvent::Enter:
        return "QEnterEvent";
    case QEvent::Wheel:
        return "QWheelEvent";
    case QEvent::HoverEnter:
    case QEvent::HoverMove:
    case QEvent::HoverLeave:
        return "QHoverEvent";
    case QEvent::DynamicPropertyChange:
        return "QDynamicPropertyChangeEvent";
    case QEvent::DeferredDelete:
        return "QDeferredDeleteEvent";
    case QEvent::ChildAdded:
    case QEvent::ChildPolished:
    case QEvent::ChildRemoved:
        return "QChildEvent";
    case QEvent::Timer:
        return "QTimerEvent";
    case QEvent::MetaCall:
        return "QMetaCallEvent";  // about to change in 5.14? see https://code.qt.io/cgit/qt/qtbase.git/commit/?h=dev&id=999c26dd83ad37fcd7a2b2fc62c0281f38c8e6e0
    default:
        return "";
    }
}


static bool eventCallback(void **data)
{
    QEvent *event = reinterpret_cast<QEvent*>(data[1]);
    QObject *receiver = reinterpret_cast<QObject*>(data[0]);

    if (!event || !receiver) {
        qWarning() << "Event or receiver is invalid";
        return false;
    }

    if (Probe::instance()->filterObject(receiver)) {
        return false;
    }

    EventData eventData;
    eventData.time = QTime::currentTime();
    eventData.type = event->type();
    eventData.receiver = receiver;
    eventData.attributes << QPair<const char*, QVariant>{"receiver", QVariant::fromValue(receiver)};

    QString className = eventTypeToClassName(event->type());
    if (!className.isEmpty()) {
        MetaObject *metaObj = MetaObjectRepository::instance()->metaObject(className);
        if (metaObj) {
            for (int i=0; i<metaObj->propertyCount(); ++i) {
                MetaProperty* prop = metaObj->propertyAt(i);
                if (strcmp(prop->name(), "type") == 0) continue;
                eventData.attributes << QPair<const char*, QVariant>{prop->name(), prop->value(event)};
            }
        }
    }

    if (s_model) {
        // add directly from foreground thread, delay from background thread
        QMetaObject::invokeMethod(s_model, "addEvent", Qt::AutoConnection,
                                  Q_ARG(GammaRay::EventData, eventData));
    }
    return false;
}


EventMonitor::EventMonitor(Probe *probe, QObject *parent)
    : QObject(parent)
    , m_eventModel(new EventModel(this))
    , m_eventPropertyModel(new AggregatedPropertyModel(this))
{
    Q_ASSERT(s_model == nullptr);
    s_model = m_eventModel;

    QInternal::registerCallback(QInternal::EventNotifyCallback, eventCallback);

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(m_eventModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventModel"), proxy);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventPropertyModel"), m_eventPropertyModel);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &EventMonitor::eventSelected);
}

void EventMonitor::eventSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex index = selection.first().topLeft();
    QVariant eventAttributes = QVariant(index.data(EventModelRole::AttributesRole).value<QVariantMap>());

    m_eventPropertyModel->setObject(eventAttributes);
}

EventMonitor::~EventMonitor() = default;
