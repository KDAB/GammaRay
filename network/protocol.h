#ifndef GAMMARAY_PROTOCOL_H
#define GAMMARAY_PROTOCOL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QModelIndex>
#include <QPair>

namespace GammaRay {

namespace Protocol {

enum MessageType {
  // client -> server
  ResolveObjectName,

  ModelRowColumnCountRequest,
  ModelContentRequest,
  ModelHeaderRequest,
  ModelSetDataRequest,

  // server -> client
  ServerVersion,
  ObjectNameReply,

  ModelRowColumnCountReply,
  ModelContentReply,
  ModelContentChanged,
  ModelHeaderChanged,
  ModelRowsAdded,
  ModelRowsRemoved,
  ModelColumnsAdded,
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

QDataStream& operator<<(QDataStream &stream, GammaRay::Protocol::MessageType messageType);
QDataStream& operator>>(QDataStream& stream, GammaRay::Protocol::MessageType& messageType);

#endif
