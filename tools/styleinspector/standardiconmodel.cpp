#include "standardiconmodel.h"

using namespace GammaRay;

StandardIconModel::StandardIconModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void StandardIconModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant StandardIconModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
  }

  return QVariant();
}

int StandardIconModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int StandardIconModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid() || !m_style)
    return 0;
  return 0;
}

QVariant StandardIconModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Name");
      case 1: return tr("Icon");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "standardiconmodel.moc"
