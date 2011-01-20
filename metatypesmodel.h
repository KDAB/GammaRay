
#ifndef METATYPESMODEL_H
#define METATYPESMODEL_H

#include <QtCore/QAbstractTableModel>

class MetaTypesModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  MetaTypesModel(QObject *parent = 0);

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& child) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

private:
  int m_lastMetaType;

};

#endif
