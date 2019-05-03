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
#include <core/util.h>

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QDebug>
#include <QItemSelectionModel>
#include <QMetaMethod>
#include <QMutex>
#include <QtCore/private/qobject_p.h>
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
        return QStringLiteral("QMouseEvent");
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
        return QStringLiteral("QTouchEvent");
    case QEvent::ScrollPrepare:
        return QStringLiteral("QScrollPrepareEvent");
    case QEvent::Scroll:
        return QStringLiteral("QScrollEvent");
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::TabletEnterProximity:
    case QEvent::TabletLeaveProximity:
        return QStringLiteral("QTabletEvent");
    case QEvent::NativeGesture:
        return QStringLiteral("QNativeGestureEvent");
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        return QStringLiteral("QKeyEvent");
    case QEvent::InputMethod:
        return QStringLiteral("QInputMethodEvent");
    case QEvent::InputMethodQuery:
        return QStringLiteral("QInputMethodQueryEvent");
    case QEvent::OrientationChange:
        return QStringLiteral("QScreenOrientationChangeEvent");
    case QEvent::WindowStateChange:
        return QStringLiteral("QWindowStateChangeEvent");
    case QEvent::ApplicationStateChange:
        return QStringLiteral("QApplicationStateChangeEvent");
    case QEvent::Expose:
        return QStringLiteral("QExposeEvent");
    case QEvent::Resize:
        return QStringLiteral("QResizeEvent");
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::FocusAboutToChange:
        return QStringLiteral("QFocusEvent");
    case QEvent::Move:
        return QStringLiteral("QMoveEvent");
    case QEvent::Paint:
        return QStringLiteral("QPaintEvent");
    case QEvent::Enter:
        return QStringLiteral("QEnterEvent");
    case QEvent::Wheel:
        return QStringLiteral("QWheelEvent");
    case QEvent::HoverEnter:
    case QEvent::HoverMove:
    case QEvent::HoverLeave:
        return QStringLiteral("QHoverEvent");
    case QEvent::DynamicPropertyChange:
        return QStringLiteral("QDynamicPropertyChangeEvent");
    case QEvent::DeferredDelete:
        return QStringLiteral("QDeferredDeleteEvent");
    case QEvent::ChildAdded:
    case QEvent::ChildPolished:
    case QEvent::ChildRemoved:
        return QStringLiteral("QChildEvent");
    case QEvent::Timer:
        return QStringLiteral("QTimerEvent");
    case QEvent::MetaCall:
        return QStringLiteral("QMetaCallEvent");  // about to change in 5.14? see https://code.qt.io/cgit/qt/qtbase.git/commit/?h=dev&id=999c26dd83ad37fcd7a2b2fc62c0281f38c8e6e0
    default:
        return QStringLiteral("");
    }
}


static bool eventCallback(void **data)
{
    if (!s_model || !Probe::instance()) {
        return false;
    }

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

    // the receiver of a deferred delete event is almost always invalid when shown in the UI
    // we therefore store the name of the receiver as a string to provide at least
    // some useful information:
    if (event->type() == QEvent::DeferredDelete) {
        if (Probe::instance()->isValidObject(receiver)) {
            eventData.attributes << QPair<const char*, QVariant>{"receiverWas", Util::displayString(receiver)};
        }
    }

    // try to extract the method name and arguments from a meta call event:
    if (event->type() == QEvent::MetaCall) {
        // about to change in 5.14? see https://code.qt.io/cgit/qt/qtbase.git/commit/?h=dev&id=999c26dd83ad37fcd7a2b2fc62c0281f38c8e6e0
        QMetaCallEvent* metaCallEvent = static_cast<QMetaCallEvent*>(event);

        if (metaCallEvent) {
            if (Probe::instance()->isValidObject(receiver)) {
                const QMetaObject *meta = receiver->metaObject();
                if (meta) {
                    QMetaMethod method = meta->method(metaCallEvent->id());
                    int argc = method.parameterCount();
                    void** argv = metaCallEvent->args();
                    QList<QVariant> vargs;
                    vargs.reserve(argc);
                    for (int i = 0; i < argc; ++i) {
                        int type = method.parameterType(i);
                        vargs.append(QVariant(type, argv[i+1]));
                    }
                    eventData.attributes << QPair<const char*, QVariant>{"methodName", method.name()};
                    eventData.attributes << QPair<const char*, QVariant>{"args", vargs};
                }
            }
        }
    }

    // store all other meta properties:
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

EventMonitor::~EventMonitor() {
    s_model = nullptr;
    QInternal::unregisterCallback(QInternal::EventNotifyCallback, eventCallback);
}
