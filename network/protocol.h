#ifndef GAMMARAY_PROTOCOL_H
#define GAMMARAY_PROTOCOL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QModelIndex>
#include <QPair>

namespace GammaRay {

namespace Protocol {


typedef quint8 ObjectAddress;
typedef quint8 MessageType;

static const ObjectAddress InvalidObjectAddress = 0;
static const MessageType InvalidMessageType = 0;

enum BuildInMessageType {
  // client -> server
  ModelRowColumnCountRequest,
  ModelContentRequest,
  ModelHeaderRequest,
  ModelSetDataRequest,
  ModelSyncBarrier,

  // server -> client
  ServerVersion,

  ObjectMapReply,
  ObjectAdded,
  ObjectRemoved,

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

  // internal
  LastMessageType
};

typedef QVector<QPair<qint32, qint32> > ModelIndex;

ModelIndex fromQModelIndex(const QModelIndex &index);

QModelIndex toQModelIndex(QAbstractItemModel *model, const ModelIndex &index);

qint32 version();

}

}

QDataStream& operator<<(QDataStream &stream, GammaRay::Protocol::BuildInMessageType messageType);
QDataStream& operator>>(QDataStream& stream, GammaRay::Protocol::BuildInMessageType& messageType);

#endif
