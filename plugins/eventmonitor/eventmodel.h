/*
  eventmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_EVENTMONITOR_EVENTMODEL_H
#define GAMMARAY_EVENTMONITOR_EVENTMODEL_H

#include <QAbstractItemModel>
#include <QTime>
#include <QVector>
#include <QEvent>
#include <QVariant>
#include <QPair>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
struct EventData
{
    QTime time;
    QEvent::Type type;
    QObject *receiver;
    QVector<QPair<const char *, QVariant>> attributes;
    QEvent *eventPtr;
    QVector<EventData> propagatedEvents;
};
}

Q_DECLARE_METATYPE(GammaRay::EventData)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::EventData, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

namespace GammaRay {
class EventModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit EventModel(QObject *parent = nullptr);
    ~EventModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    bool hasEvents() const;
    EventData &lastEvent();

public slots:
    void addEvent(const GammaRay::EventData &event);

    void clear();

private:
    QVector<EventData> m_events;
    QVector<EventData> m_pendingEvents;
    QTimer *m_pendingEventTimer;
};
}

#endif // GAMMARAY_EVENTMONITOR_EVENTMODEL_H
