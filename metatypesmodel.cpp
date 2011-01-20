
#include "metatypesmodel.h"

#include <QtCore/QMetaType>
#include <QtCore/QDebug>

MetaTypesModel::MetaTypesModel(QObject* parent)
  : QAbstractItemModel(parent), m_lastMetaType(0)
{
  qDebug() << "REG" << QMetaType::isRegistered(0);
  for (m_lastMetaType = 0; ; ++m_lastMetaType ) {
    if (!QMetaType::isRegistered(m_lastMetaType))
      break;
  }
  qDebug() << "DONE" << m_lastMetaType;
}

QVariant MetaTypesModel::data(const QModelIndex& index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (index.column() == 0) {
    return QMetaType::typeName(index.row());
  } else if (index.column() == 1) {
    return index.row();
  }
  return QVariant();
}

int MetaTypesModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return m_lastMetaType;
}

int MetaTypesModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return 2;
}

QModelIndex MetaTypesModel::index(int row, int column, const QModelIndex& parent) const
{
  return createIndex(row, column);
}

QModelIndex MetaTypesModel::parent(const QModelIndex& child) const
{
  return QModelIndex();
}

QVariant MetaTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (section == 0)
    return "typeName";
  return "MetaTypeId";
}


#include "metatypesmodel.moc"
