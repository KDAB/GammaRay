/*
  eventmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#ifndef GAMMARAY_EVENTMONITOR_EVENTMODEL_H
#define GAMMARAY_EVENTMONITOR_EVENTMODEL_H

#include <QAbstractItemModel>
#include <QTime>
#include <QVector>
#include <QEvent>
#include <QVariant>
#include <QPair>

namespace GammaRay {
struct EventData {
    QTime time;
    QEvent::Type type;
    QObject* receiver;
    QVector<QPair<const char *, QVariant>> attributes;
    QEvent* eventPtr;
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
    QMap<int, QVariant> itemData(const QModelIndex & index) const override;

public slots:
    void addEvent(const GammaRay::EventData &event);

    void clear();

private:
    friend class EventPropagationListener;
    QVector<EventData> m_events;
};
}

#endif // GAMMARAY_EVENTMONITOR_EVENTMODEL_H

