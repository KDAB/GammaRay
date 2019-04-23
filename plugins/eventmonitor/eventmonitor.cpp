/*
  styleinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "eventmonitor.h"

#include <core/remote/serverproxymodel.h>

#include <common/objectbroker.h>

#include <QDebug>
#include <QMutex>
#include <QSortFilterProxyModel>

using namespace GammaRay;


static EventModel *s_model = nullptr;


static bool eventCallback(void **data)
{
    QEvent *event = reinterpret_cast<QEvent*>(data[1]);
    QObject *receiver = reinterpret_cast<QObject*>(data[0]);
    if (!event || !receiver) {
        qWarning() << "Event or receiver is invalid";
        return false;
    }

    EventData eventData;
    eventData.time = QTime::currentTime();
    eventData.type = event->type();
    eventData.spontaneous = event->spontaneous();
    eventData.accepted = event->isAccepted();
    eventData.receiver = receiver;

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
{
    Q_ASSERT(s_model == nullptr);
    s_model = m_eventModel;

    QInternal::registerCallback(QInternal::EventNotifyCallback, eventCallback);

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(m_eventModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EventModel"), proxy);
}

EventMonitor::~EventMonitor() = default;
