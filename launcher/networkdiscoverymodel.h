#ifndef GAMMARAY_NETWORKDISCOVERYMODEL_H
#define GAMMARAY_NETWORKDISCOVERYMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QDateTime>

class QUdpSocket;

namespace GammaRay {

/** Lists all active server instances found via network autodiscovery. */
class NetworkDiscoveryModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  enum Role {
    HostNameRole = Qt::UserRole + 1,
    PortRole
  };

  explicit NetworkDiscoveryModel(QObject* parent);
  ~NetworkDiscoveryModel();

  QVariant data(const QModelIndex& index, int role) const;
  int columnCount(const QModelIndex& parent) const;
  int rowCount(const QModelIndex& parent) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;

private slots:
  void processPendingDatagrams();
  void expireEntries();

private:
  QUdpSocket *m_socket;

  struct ServerInfo {
    bool operator==(const ServerInfo &other);
    qint32 version;
    QString host;
    quint16 port;
    QString label;
    QDateTime lastSeen;
  };
  QVector<ServerInfo> m_data;

};

}

#endif // GAMMARAY_NETWORKDISCOVERYMODEL_H
