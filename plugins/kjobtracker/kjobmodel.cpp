#include "kjobmodel.h"
#include <include/util.h>

#include <KJob>

using namespace GammaRay;

KJobModel::KJobModel(QObject* parent): QAbstractTableModel(parent)
{
}

QVariant KJobModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const KJobInfo& job = m_data.at(index.row());
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0: return job.name;
      case 1: return job.type;
    }
  }

  return QVariant();
}

int KJobModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int KJobModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_data.size();
}

QVariant KJobModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Job");
      case 1: return tr("Type");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

void KJobModel::objectAdded(QObject* obj)
{
  KJob *job = qobject_cast<KJob*>(obj);
  if (!job)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  KJobInfo jobInfo;
  jobInfo.name = obj->objectName().isEmpty() ? Util::addressToString(obj) : obj->objectName();
  jobInfo.type = obj->metaObject()->className();
  jobInfo.alive = true;
  m_data.push_back(jobInfo);
  endInsertRows();
}

#include "kjobmodel.moc"
