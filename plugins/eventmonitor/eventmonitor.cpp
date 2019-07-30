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

#include "eventmodel.h"
#include "eventmodelroles.h"
#include "eventmonitorinterface.h"
#include "eventtypefilter.h"
#include "eventtypemodel.h"

#include <core/aggregatedpropertymodel.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objectinstance.h>
#include <core/remote/serverproxymodel.h>
#include <core/util.h>

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QItemSelectionModel>
#include <QMetaMethod>
#include <QMutex>
#include <QSortFilterProxyModel>
#include <QtCore/private/qobject_p.h>

using namespace GammaRay;

static EventModel *s_model = nullptr;
static EventTypeModel *s_eventTypeModel = nullptr;
static EventMonitor *s_eventMonitor = nullptr;


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
    case QEvent::Shortcut:
        return QStringLiteral("QShortcutEvent");
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
    case QEvent::ActionAdded:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
        return QStringLiteral("QActionEvent");
    case QEvent::ContextMenu:
        return QStringLiteral("QContextMenuEvent");
    case QEvent::Drop:
        return QStringLiteral("QDropEvent");
    case QEvent::DragEnter:
    case QEvent::DragMove:
        return QStringLiteral("QDragMoveEvent");
    case QEvent::GraphicsSceneHelp:
    case QEvent::QueryWhatsThis:
    case QEvent::ToolTip:
        return QStringLiteral("QHelpEvent");
    case QEvent::StatusTip:
        return QStringLiteral("QStatusTip");
    default:
        return QString();
    }
}


bool shouldBeRecorded(QObject* receiver, QEvent* event) {
    if (!s_model || !s_eventTypeModel || !s_eventMonitor || !Probe::instance()) {
        return false;
    }
    if (s_eventMonitor->isPaused()) {
        return false;
    }
    if (!event || !receiver) {
        return false;
    }
    if (!s_eventTypeModel->isRecording(event->type())) {
        return false;
    }
    if (Probe::instance()->filterObject(receiver)) {
        return false;
    }
    return true;
}


EventData createEventData(QObject* receiver, QEvent* event) {
    EventData eventData;
    eventData.time = QTime::currentTime();
    eventData.type = event->type();
    eventData.receiver = receiver;
    eventData.attributes << QPair<const char*, QVariant>{"receiver", QVariant::fromValue(receiver)};
    eventData.eventPtr = event;

    // the receiver of a deferred delete event is almost always invalid when shown in the UI
    // we therefore store the name of the receiver as a string to provide at least
    // some useful information:
    if (event->type() == QEvent::DeferredDelete) {
        eventData.attributes << QPair<const char*, QVariant>{"[receiver type]", Util::displayString(receiver)};
    }

    // try to extract the method name, arguments and return value from a meta call event:
    if (event->type() == QEvent::MetaCall) {
        eventData.attributes << QPair<const char*, QVariant>{"[receiver type]", Util::displayString(receiver)};
        // QMetaCallEvent about to change in 5.14? see https://code.qt.io/cgit/qt/qtbase.git/commit/?h=dev&id=999c26dd83ad37fcd7a2b2fc62c0281f38c8e6e0
        QMetaCallEvent* metaCallEvent = static_cast<QMetaCallEvent*>(event);
        if (metaCallEvent) {
            int methodIndex = metaCallEvent->id();
            if (methodIndex == int(ushort(-1))) {
                // TODO: this is a slot call, but QMetaCall::slotObj is private
                eventData.attributes << QPair<const char*, QVariant>{"[method name]", "[unknown slot]"};
            } else {
                // TODO: should first check if nargs and types is set, but both are private
                const QMetaObject *meta = receiver->metaObject();
                if (meta) {
                    QMetaMethod method = meta->method(metaCallEvent->id());
                    eventData.attributes << QPair<const char*, QVariant>{"[method name]", method.name()};
                    void** argv = metaCallEvent->args();
                    if (argv) { // nullptr e.g. for QDBusCallDeliveryEvent
                        if (method.returnType() != QMetaType::Void) {
                            void* returnValueCopy = QMetaType::create(method.returnType(), argv[0]);
                            eventData.attributes << QPair<const char*, QVariant>{"[return value]", QVariant(method.returnType(), returnValueCopy)};
                        }
                        int argc = method.parameterCount();
                        QVariantMap vargs;
                        for (int i = 0; i < argc; ++i) {
                            int type = method.parameterType(i);
                            void* argumentDataCopy = QMetaType::create(type, argv[i+1]);
                            vargs.insert(method.parameterNames().at(i), QVariant(type, argumentDataCopy));
                        }
                        if (argc > 0)
                            eventData.attributes << QPair<const char*, QVariant>{"[arguments]", vargs};
                    }
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
    return eventData;
}

void EventMonitor::addEvent(const GammaRay::EventData &event)
{
    m_eventModel->addEvent(event);
    m_eventTypeModel->increaseCount(event.type);
}

static bool eventCallback(void **data)
{
    QEvent *event = reinterpret_cast<QEvent*>(data[1]);
    QObject *receiver = reinterpret_cast<QObject*>(data[0]);

    if (!shouldBeRecorded(receiver, event))
        return false;

    EventData eventData = createEventData(receiver, event);

    if (!event->spontaneous()
            && s_model->hasEvents()
            && s_model->lastEvent().eventPtr == eventData.eventPtr
            && s_model->lastEvent().type == event->type()) {
        // this is an event propagated by a QQuickWindow to a child item:
        s_model->lastEvent().propagatedEvents.append(eventData);
        return false;
    }

    // add directly from foreground thread, delay from background thread
    QMetaObject::invokeMethod(s_eventMonitor, "addEvent", Qt::AutoConnection, Q_ARG(GammaRay::EventData, eventData));
    return false;
}


EventPropagationListener::EventPropagationListener(QObject *parent)
    : QObject(parent)
{}

bool EventPropagationListener::eventFilter(QObject *receiver, QEvent *event)
{
    if (!s_model)
        return false;

    if (!s_model->hasEvents())
        return false;

    EventData& lastEvent = s_model->lastEvent();

    if (lastEvent.eventPtr == event && lastEvent.receiver == receiver) {
        // this is the same event we already recorded in the event callback
        return false;
    }
    if (!lastEvent.propagatedEvents.isEmpty()
            && lastEvent.propagatedEvents.last().eventPtr == event) {
        // this is an event propagated by QML that is already recorded in the event callback
        return false;
    }

    if (!shouldBeRecorded(receiver, event))
        return false;

    if (event->type() != lastEvent.type) {
        // a new event was created during the propagation
        EventData newEvent = createEventData(receiver, event);
        s_model->addEvent(newEvent);
        s_eventTypeModel->increaseCount(event->type());
        return false;
    }

    EventData propagatedEvent = createEventData(receiver, event);
    lastEvent.propagatedEvents.append(propagatedEvent);

    return false;
}


EventMonitor::EventMonitor(Probe *probe, QObject *parent)
    : EventMonitorInterface(parent)
    , m_eventModel(new EventModel(this))
    , m_eventTypeModel(new EventTypeModel(this))
    , m_eventPropertyModel(new AggregatedPropertyModel(this))
{
    Q_ASSERT(s_model == nullptr);
    s_model = m_eventModel;

    Q_ASSERT(s_eventTypeModel == nullptr);
    s_eventTypeModel = m_eventTypeModel;

    Q_ASSERT(s_eventMonitor == nullptr);
    s_eventMonitor = this;

    QInternal::registerCallback(QInternal::EventNotifyCallback, eventCallback);
    QCoreApplication::instance()->installEventFilter(new EventPropagationListener(this));

    auto filterProxy = new ServerProxyModel<EventTypeFilter>(this);
    filterProxy->setEventTypeModel(m_eventTypeModel);
    filterProxy->setSourceModel(m_eventModel);
    connect(m_eventTypeModel, &EventTypeModel::typeVisibilityChanged, filterProxy, &QSortFilterProxyModel::invalidate);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventModel"), filterProxy);

    auto evenTypeProxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    evenTypeProxy->setSourceModel(m_eventTypeModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventTypeModel"), evenTypeProxy);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventPropertyModel"), m_eventPropertyModel);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(filterProxy);
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
    s_eventTypeModel = nullptr;
    s_eventMonitor = nullptr;
    QInternal::unregisterCallback(QInternal::EventNotifyCallback, eventCallback);
}

void EventMonitor::clearHistory()
{
    m_eventModel->clear();
    m_eventTypeModel->resetCounts();
}

void EventMonitor::recordAll()
{
    m_eventTypeModel->recordAll();
}

void EventMonitor::recordNone()
{
    m_eventTypeModel->recordNone();
}

void EventMonitor::showAll()
{
    m_eventTypeModel->showAll();
}

void EventMonitor::showNone()
{
    m_eventTypeModel->showNone();
}
