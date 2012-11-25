#ifndef GAMMARAY_PROTOCOL_H
#define GAMMARAY_PROTOCOL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QModelIndex>
#include <QPair>

namespace GammaRay {

namespace Protocol {

enum ModelRequest {
  RowColumnCountRequest,
  ContentRequest,
  HeaderRequest,
  SetDataRequest
};

enum ModelReply {
  RowColumnCountReply,
  ContentChanged,
  HeaderChanged,
  RowsAdded,
  RowsRemoved,
  ColumnsAdded,
  ColumnsRemoved,
  ModelReset,
  ModelLayoutChanged
};

typedef QVector<QPair<qint32, qint32> > ModelIndex;

ModelIndex fromQModelIndex(const QModelIndex &index);

QModelIndex toQModelIndex(QAbstractItemModel *model, const ModelIndex &index);

}

}

#endif
