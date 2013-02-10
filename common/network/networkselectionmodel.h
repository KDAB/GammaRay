#ifndef GAMMARAY_NETWORKSELECTIONMODEL_H
#define GAMMARAY_NETWORKSELECTIONMODEL_H

#include <QItemSelectionModel>
#include "protocol.h"

namespace GammaRay {

class Message;

/** Base class for network-transparent item selection models, do not use directly. */
class NetworkSelectionModel : public QItemSelectionModel
{
  Q_OBJECT
public:
  ~NetworkSelectionModel();

protected:
  explicit NetworkSelectionModel(const QString &objectName, QAbstractItemModel *model, QObject *parent = 0);
  QString m_objectName;
  Protocol::ObjectAddress m_myAddress;

private slots:
  void newMessage(const GammaRay::Message &msg);

  void slotCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
  void slotCurrentColumnChanged(const QModelIndex &current, const QModelIndex &previous);
  void slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
  void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

};

}

#endif // GAMMARAY_NETWORKSELECTIONMODEL_H
