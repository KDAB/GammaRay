/*
  eventtypemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#ifndef GAMMARAY_EVENTMONITOR_EVENTTYPEMODEL_H
#define GAMMARAY_EVENTMONITOR_EVENTTYPEMODEL_H

#include <core/execution.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QEvent>

namespace GammaRay {
struct EventTypeData {
    int count = 0;
    bool loggingEnabled = true;
};
}

Q_DECLARE_METATYPE(GammaRay::EventTypeData)
QT_BEGIN_NAMESPACE
    Q_DECLARE_TYPEINFO(GammaRay::EventTypeData, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

namespace GammaRay {
class EventTypeModel : public QAbstractTableModel
{
    Q_OBJECT

    enum Columns {
        Type = 0,
        Count,
        LoggingStatus,
        COUNT
    };

public:
    explicit EventTypeModel(QObject *parent = nullptr);
    ~EventTypeModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void increaseCount(QEvent::Type type);

    void enableAll();
    void disableAll();

    void resetCount();

private:
    void initEventTypes();

private:
    QMap<QEvent::Type, EventTypeData*> m_data;
};
}

#endif // GAMMARAY_EVENTMONITOR_EVENTTYPEMODEL_H

