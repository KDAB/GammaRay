#ifndef ENDOSCOPE_CONNECTIONFILTERPROXYMODEL_H
#define ENDOSCOPE_CONNECTIONFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>


namespace Endoscope {

class ConnectionFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
  public:
    ConnectionFilterProxyModel(QObject* parent = 0);

    void filterReceiver( QObject * receiver );
    void filterSender( QObject *sender );

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

  private:
    QObject *m_receiver;
    QObject *m_sender;
};

}

#endif // ENDOSCOPE_CONNECTIONFILTERPROXYMODEL_H
