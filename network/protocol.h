#ifndef GAMMARAY_PROTOCOL_H
#define GAMMARAY_PROTOCOL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QModelIndex>
#include <QPair>

namespace GammaRay {

namespace Protocol {

typedef qint32 PayloadSize;
typedef quint8 ObjectAddress;
typedef quint8 MessageType;

static const ObjectAddress InvalidObjectAddress = 0;
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
  ModelLayoutChanged
};

typedef QVector<QPair<qint32, qint32> > ModelIndex;

ModelIndex fromQModelIndex(const QModelIndex &index);

QModelIndex toQModelIndex(const QAbstractItemModel *model, const ModelIndex &index);

/** Protocol version, must match exactly between client and server. */
qint32 version();

/** Broadcast format version. */
qint32 broadcastFormatVersion();

}

}

#endif
