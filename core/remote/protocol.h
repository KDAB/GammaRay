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

inline ModelIndex fromQModelIndex(const QModelIndex &index)
{
  if (!index.isValid())
    return ModelIndex();
  ModelIndex result = fromQModelIndex(index.parent());
  result.push_back(qMakePair(index.row(), index.column()));
  return result;
}

inline QModelIndex toQModelIndex(QAbstractItemModel *model, const ModelIndex &index)
{
  QModelIndex qmi;

  for (int i = 0; i < index.size(); ++i)
    qmi = model->index(index.at(i).first, index.at(i).second, qmi);

  return qmi;
}

}

}

#endif
