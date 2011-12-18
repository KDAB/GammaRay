
#include "localeaccessormodel.h"
#include <qmath.h>
#include "localedataaccessor.h"

#include <QDebug>

using namespace GammaRay;

LocaleAccessorModel::LocaleAccessorModel(QObject* parent)
  : QAbstractTableModel(parent)
{

}

int LocaleAccessorModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  int area = LocaleDataAccessorRegistry::accessors().size();
  return qSqrt(area);
}

int LocaleAccessorModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  int area = LocaleDataAccessorRegistry::accessors().size();
  return qCeil((float)area / (int)qSqrt(area));
}

Qt::ItemFlags LocaleAccessorModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant LocaleAccessorModel::data(const QModelIndex& index, int role) const
{
  QVector< LocaleDataAccessor* > acc = LocaleDataAccessorRegistry::accessors();
  int offset = (index.row() * columnCount()) + index.column();
  if (offset >= acc.size())
    return QVariant();

  LocaleDataAccessor* accessor = acc.at(offset);
  switch(role) {
  case Qt::DisplayRole:
    return accessor->accessorName();
  case Qt::CheckStateRole:
    return LocaleDataAccessorRegistry::enabledAccessors().contains(accessor) ? Qt::Checked : Qt::Unchecked;
  case AccessorRole:
    return QVariant::fromValue<LocaleDataAccessor*>(accessor);
  default:
    return QVariant();
  }
}

bool LocaleAccessorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role != Qt::CheckStateRole)
    return QAbstractItemModel::setData(index, value, role);
  bool enabled = value.toInt() == Qt::Checked;
  LocaleDataAccessor *accessor = index.data(AccessorRole).value<LocaleDataAccessor*>();
  LocaleDataAccessorRegistry::setAccessorEnabled(accessor, enabled);
  return false;
}

#include "localeaccessormodel.moc"
