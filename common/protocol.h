/*
  protocol.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include <QVector>
#include <QModelIndex>
#include <QPair>

namespace GammaRay {

/** @brief Helper functions and constants defining the communication protocol between client and server. */
namespace Protocol {

typedef qint32 PayloadSize;
typedef quint8 ObjectAddress;
typedef quint8 MessageType;

static const ObjectAddress InvalidObjectAddress = 0;
static const ObjectAddress LauncherAddress = 255;
static const MessageType InvalidMessageType = 0;

enum BuildInMessageType {
  // object management
  // client -> server
  ObjectMonitored = InvalidMessageType + 1,
  ObjectUnmonitored,

  // server -> client
  ServerVersion,

  ObjectMapReply,
  ObjectAdded,
  ObjectRemoved,

  // remote model messages
  // client -> server
  ModelRowColumnCountRequest,
  ModelContentRequest,
  ModelHeaderRequest,
  ModelSetDataRequest,
  ModelSyncBarrier,

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

  ServerInfo,

  // probe settings provided by the launcher
  ProbeSettings,
  ServerPort
};

typedef QVector<QPair<qint32, qint32> > ModelIndex;

/** Serializes a QModelIndex. */
GAMMARAY_COMMON_EXPORT ModelIndex fromQModelIndex(const QModelIndex &index);

/** Deserializes a QModelIndex. */
GAMMARAY_COMMON_EXPORT QModelIndex toQModelIndex(const QAbstractItemModel *model, const ModelIndex &index);

/** Protocol version, must match exactly between client and server. */
GAMMARAY_COMMON_EXPORT qint32 version();

/** Broadcast format version. */
GAMMARAY_COMMON_EXPORT qint32 broadcastFormatVersion();

}

}

#endif
