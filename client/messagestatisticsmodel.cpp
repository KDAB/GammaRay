/*
  messagestatisticsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "messagestatisticsmodel.h"

#include <ui/uiintegration.h>

#include <core/metaenum.h>

#include <algorithm>
#include <numeric>

using namespace GammaRay;

#define M(x) { Protocol:: x, #x }
static const MetaEnum::Value<Protocol::MessageType> message_type_table[] = {
    M(ObjectMonitored),
    M(ObjectUnmonitored),
    M(ServerVersion),
    M(ServerDataVersionNegotiated),
    M(ObjectMapReply),
    M(ObjectAdded),
    M(ObjectRemoved),
    M(ClientDataVersionNegotiated),
    M(ModelRowColumnCountRequest),
    M(ModelContentRequest),
    M(ModelHeaderRequest),
    M(ModelSetDataRequest),
    M(ModelSortRequest),
    M(ModelSyncBarrier),
    M(SelectionModelStateRequest),
    M(ModelRowColumnCountReply),
    M(ModelContentReply),
    M(ModelContentChanged),
    M(ModelHeaderReply),
    M(ModelHeaderChanged),
    M(ModelRowsAdded),
    M(ModelRowsMoved),
    M(ModelRowsRemoved),
    M(ModelColumnsAdded),
    M(ModelColumnsMoved),
    M(ModelColumnsRemoved),
    M(ModelReset),
    M(ModelLayoutChanged),
    M(SelectionModelSelect),
    M(SelectionModelCurrent),
    M(MethodCall),
    M(PropertySyncRequest),
    M(PropertyValuesChanged),
    M(ServerInfo),
    M(ProbeSettings),
    M(ServerAddress),
    M(ServerLaunchError)
};
#undef M
Q_STATIC_ASSERT(Protocol::MESSAGE_TYPE_COUNT - 1 == (sizeof(message_type_table) / sizeof(MetaEnum::Value<Protocol::MessageType>)));

MessageStatisticsModel::Info::Info()
{
    messageCount.resize(Protocol::MESSAGE_TYPE_COUNT);
    messageSize.resize(Protocol::MESSAGE_TYPE_COUNT);
}

int MessageStatisticsModel::Info::totalCount() const
{
    return std::accumulate(messageCount.begin(), messageCount.end(), 0);
}

quint64 MessageStatisticsModel::Info::totalSize() const
{
    return std::accumulate(messageSize.begin(), messageSize.end(), 0ull);
}

MessageStatisticsModel::MessageStatisticsModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_totalCount(0)
    , m_totalSize(0)
{
}

MessageStatisticsModel::~MessageStatisticsModel() = default;

void MessageStatisticsModel::clear()
{
    beginResetModel();
    m_data.clear();
    m_totalCount = 0;
    m_totalSize = 0;
    endResetModel();
}

void MessageStatisticsModel::addObject(Protocol::ObjectAddress addr, const QString &name)
{
    addr -= 1;
    if (addr < m_data.size()) {
        m_data[addr].name = name;
        emit dataChanged(index(addr, 0), index(addr, 0));
    } else {
        beginInsertRows(QModelIndex(), m_data.size(), addr);
        m_data.resize(addr + 1);
        m_data[addr].name = name;
        endInsertRows();
    }
}

void MessageStatisticsModel::addMessage(Protocol::ObjectAddress addr, Protocol::MessageType msgType, int size)
{
    addr -= 1;
    msgType -= 1;

    ++m_totalCount;
    m_totalSize += size;

    if (addr < m_data.size()) {
        m_data[addr].messageCount[msgType]++;
        m_data[addr].messageSize[msgType] += size;
        emit dataChanged(index(addr, msgType + 1), index(addr, msgType + 1));
    } else {
        beginInsertRows(QModelIndex(), m_data.size(), addr);
        m_data.resize(addr + 1);
        m_data[addr].messageCount[msgType] = 1;
        m_data[addr].messageSize[msgType] = size;
        endInsertRows();
    }
}

int MessageStatisticsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Protocol::MESSAGE_TYPE_COUNT - 1;
}

int MessageStatisticsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_data.size();
}

// 1 / GRADIENT_SCALE_FACTOR is yellow, 2 / GRADIENT_SCALE_FACTOR and beyond is red
static const int GRADIENT_SCALE_FACTOR = 4;

static QColor colorForRatio(double ratio)
{
    const auto red = qBound<qreal>(0.0, ratio * GRADIENT_SCALE_FACTOR, 0.5);
    const auto green = qBound<qreal>(0.0, 1 - ratio * GRADIENT_SCALE_FACTOR, 0.5);
    auto color = QColor(255 * red, 255 * green, 0);
    if (!UiIntegration::hasDarkUI()) {
        return color.lighter(300);
    }
    return color;
}

QVariant MessageStatisticsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_data.isEmpty())
        return QVariant();

    const auto &info = m_data.at(index.row());
    const auto msgType = index.column();

    if (role == Qt::DisplayRole) {
        return QString(QString::number(info.messageCount[msgType])
                       + QStringLiteral(" / ")
                       + QString::number(info.messageSize[msgType]));
    }

    if (role == Qt::BackgroundRole && m_totalCount > 0 && m_totalSize > 0) {
        if (info.messageCount[msgType] == 0) {
            return QVariant();
        }
        const auto countRatio = (double)info.messageCount[msgType] / (double)m_totalCount;
        const auto sizeRatio = (double)info.messageSize[msgType] / (double)m_totalSize;
        const auto ratio = std::max(countRatio, sizeRatio);
        return colorForRatio(ratio);
    }

    if (role == Qt::ToolTipRole) {
        return tr(
            "Object: %1\nMessage Type: %2\nMessage Count: %3 of %4 (%5%)\nMessage Size: %6 of %7 (%8%)").
               arg(info.name).
               arg(MetaEnum::enumToString(static_cast<Protocol::MessageType>(index.column() + 1), message_type_table)).
               arg(info.messageCount[msgType]).
               arg(m_totalCount).
               arg(100.0 * (double)info.messageCount[msgType] / (double)m_totalCount, 0, 'f', 2).
               arg(info.messageSize[msgType]).
               arg(m_totalSize).
               arg(100.0 * (double)info.messageSize[msgType] / (double)m_totalSize, 0, 'f', 2);
    }

    return QVariant();
}

QVariant MessageStatisticsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole)
            return MetaEnum::enumToString(static_cast<Protocol::MessageType>(section + 1), message_type_table);

        if (role == Qt::BackgroundRole) {
            const auto countRatio = (double)countPerType(section) / (double)m_totalCount;
            const auto sizeRatio = (double)sizePerType(section) / (double)m_totalSize;
            const auto ratio = std::max(countRatio, sizeRatio);
            if (ratio > 0.0) {
                return colorForRatio(ratio);
            }
        }

        if (role == Qt::ToolTipRole) {
            const auto count = countPerType(section);
            const auto size = sizePerType(section);
            return tr("Message Count: %1 of %2 (%3%)\nMessage Size: %4 of %5 (%6%)").
                   arg(count).
                   arg(m_totalCount).
                   arg(100.0 * (double)count / (double)m_totalCount, 0, 'f', 2).
                   arg(size).
                   arg(m_totalSize).
                   arg(100.0 * (double)size / (double)m_totalSize, 0, 'f', 2);
        }
    } else if (orientation == Qt::Vertical) {
        const auto &info = m_data.at(section);
        if (role == Qt::DisplayRole) {
            return info.name;
        }
        if (role == Qt::BackgroundRole) {
            const auto countRatio = (double)info.totalCount() / (double)m_totalCount;
            const auto sizeRatio = (double)info.totalSize() / (double)m_totalSize;
            const auto ratio = std::max(countRatio, sizeRatio);
            if (ratio > 0.0) {
                return colorForRatio(ratio);
            }
        }
        if (role == Qt::ToolTipRole) {
            const auto count = info.totalCount();
            const auto size = info.totalSize();
            return tr("Message Count: %1 of %2 (%3%)\nMessage Size: %4 of %5 (%6%)\nObject Address: %7").
                   arg(count).
                   arg(m_totalCount).
                   arg(100.0 * (double)count / (double)m_totalCount, 0, 'f', 2).
                   arg(size).
                   arg(m_totalSize).
                   arg(100.0 * (double)size / (double)m_totalSize, 0, 'f', 2).
                   arg(section + 1);
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

int MessageStatisticsModel::countPerType(int msgType) const
{
    int c = 0;
    for (const auto &info : m_data) {
        c += info.messageCount.at(msgType);
    }
    return c;
}

quint64 MessageStatisticsModel::sizePerType(int msgType) const
{
    int c = 0;
    for (const auto &info : m_data) {
        c += info.messageSize.at(msgType);
    }
    return c;
}
