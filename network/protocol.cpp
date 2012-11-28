#include "protocol.h"

namespace GammaRay {

namespace Protocol {

Protocol::ModelIndex fromQModelIndex(const QModelIndex& index)
{
  if (!index.isValid())
    return ModelIndex();
  ModelIndex result = fromQModelIndex(index.parent());
  result.push_back(qMakePair(index.row(), index.column()));
  return result;
}

QModelIndex toQModelIndex(QAbstractItemModel* model, const Protocol::ModelIndex& index)
{
  QModelIndex qmi;

  for (int i = 0; i < index.size(); ++i)
    qmi = model->index(index.at(i).first, index.at(i).second, qmi);

  return qmi;
}

qint32 version()
{
  return 1;
}

}

}

QDataStream& operator<<(QDataStream& stream, GammaRay::Protocol::MessageType messageType)
{
  Q_ASSERT(GammaRay::Protocol::LastMessageType <= 255); // TODO static assert
  stream << static_cast<quint8>(messageType);
  return stream;
}


QDataStream& operator>>(QDataStream& stream, GammaRay::Protocol::MessageType &messageType)
{
  quint8 value;
  stream >> value;
  Q_ASSERT(value < GammaRay::Protocol::LastMessageType);
  messageType = static_cast<GammaRay::Protocol::MessageType>(value);
  return stream;
}
