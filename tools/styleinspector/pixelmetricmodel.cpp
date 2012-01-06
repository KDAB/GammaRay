#include "pixelmetricmodel.h"

using namespace GammaRay;

PixelMetricModel::PixelMetricModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void PixelMetricModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant PixelMetricModel::data(const QModelIndex& index, int role) const
{
  return QVariant();
}

int PixelMetricModel::columnCount(const QModelIndex& parent) const
{
  return 2;
}

int PixelMetricModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return 0;
}

QVariant PixelMetricModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Metric");
      case 1: return tr("Default Value");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "pixelmetricmodel.moc"
