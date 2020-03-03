/*
  protocol.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROTOCOL_H
#define GAMMARAY_PROTOCOL_H

#include "gammaray_common_export.h"

#include <QAbstractItemModel>
#include <QDataStream>
#include <QDebug>
#include <QVector>
#include <QModelIndex>

#include <limits>

namespace GammaRay {
/*! Helper functions and constants defining the communication protocol between client and server. */
namespace Protocol {
/*! Message payload size type. */
using PayloadSize = qint32;
/*! Remote object address type. */
using ObjectAddress = quint16;
/*! Message type type. */
using MessageType = quint8;

/*! Invalid object address. */
static const ObjectAddress InvalidObjectAddress = 0;
/*! Address of the launcher remote object for probe <-> launcher communication. */
static const ObjectAddress LauncherAddress = std::numeric_limits<ObjectAddress>::max();
/*! Invalid message type. */
static const MessageType InvalidMessageType = 0;

/*! Protocol message types. */
enum BuildInMessageType {
    // object management
    // client -> server
    ObjectMonitored = InvalidMessageType + 1,
    ObjectUnmonitored,

    // server -> client
    ServerVersion,
    ServerDataVersionNegotiated,

    ObjectMapReply,
    ObjectAdded,
    ObjectRemoved,

    // remote model messages
    // client -> server
    ClientDataVersionNegotiated,
    ModelRowColumnCountRequest,
    ModelContentRequest,
    ModelHeaderRequest,
    ModelSetDataRequest,
    ModelSortRequest,
    ModelSyncBarrier,
    SelectionModelStateRequest,

    // server -> client
    ModelRowColumnCountReply,
    ModelContentReply,
    ModelContentChanged,
    ModelHeaderReply,
    ModelHeaderChanged,
    ModelRowsAdded,
    ModelRowsMoved,
    ModelRowsRemoved,
    ModelColumnsAdded,
    ModelColumnsMoved,
    ModelColumnsRemoved,
    ModelReset,
    ModelLayoutChanged,

    // server <-> client
    SelectionModelSelect,
    SelectionModelCurrent,

    MethodCall,
    PropertySyncRequest,
    PropertyValuesChanged,

    ServerInfo,

    // probe settings provided by the launcher
    ProbeSettings,
    ServerAddress,
    ServerLaunchError,

    MESSAGE_TYPE_COUNT // NOTE when changing this enum, also update MessageStatisticsModel!
};

///@cond internal
/*! Transport protocol representation of a model index element. */
class ModelIndexData
{
public:
    explicit ModelIndexData(qint32 row_ = 0, qint32 column_ = 0)
        : row(row_), column(column_) {}

    qint32 row;
    qint32 column;
};
/*! Transport protocol representation of a QModelIndex. */
using ModelIndex = QVector<ModelIndexData>;

/*! Protocol representation of an QItemSelectionRange. */
struct ItemSelectionRange {
    ModelIndex topLeft;
    ModelIndex bottomRight;
};
/*! Protocol representation of an QItemSelection. */
using ItemSelection = QVector<ItemSelectionRange>;

/*! Serializes a QModelIndex. */
GAMMARAY_COMMON_EXPORT ModelIndex fromQModelIndex(const QModelIndex &index);

/*! Deserializes a QModelIndex. */
GAMMARAY_COMMON_EXPORT QModelIndex toQModelIndex(const QAbstractItemModel *model,
                                                 const ModelIndex &index);
///@endcond

/*! Protocol version, must match exactly between client and server. */
GAMMARAY_COMMON_EXPORT qint32 version();

/*! Broadcast format version. */
GAMMARAY_COMMON_EXPORT qint32 broadcastFormatVersion();
}
}

///@cond internal
QT_BEGIN_NAMESPACE
inline QDataStream& operator>>(QDataStream& s, GammaRay::Protocol::ModelIndexData& data)
{
    s >> data.row >> data.column;
    return s;
}
inline QDataStream& operator<<(QDataStream& s, const GammaRay::Protocol::ModelIndexData& data)
{
    s << data.row << data.column;
    return s;
}

inline QDebug& operator<<(QDebug &s, const GammaRay::Protocol::ModelIndexData &data)
{
    s << '(' << data.row << ',' << data.column << ')';
    return s;
}
///@endcond

Q_DECLARE_TYPEINFO(GammaRay::Protocol::ModelIndexData, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(GammaRay::Protocol::ItemSelectionRange, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

#endif
