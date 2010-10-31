#ifndef ENDOSCOPE_MODELCELLMODEL_H
#define ENDOSCOPE_MODELCELLMODEL_H

#include <QAbstractTableModel>

namespace Endoscope {

class ModelCellModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit ModelCellModel( QObject *parent = 0 );
    void setModelIndex( const QModelIndex &index );

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private:
    QPersistentModelIndex m_index;
};

}

#endif // ENDOSCOPE_MODELCELLMODEL_H
