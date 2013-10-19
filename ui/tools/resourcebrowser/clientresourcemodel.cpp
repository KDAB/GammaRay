#include "clientresourcemodel.h"

#include <QIcon>

using namespace GammaRay;

ClientResourceModel::ClientResourceModel(QObject* parent): QSortFilterProxyModel(parent)
{
}

ClientResourceModel::~ClientResourceModel()
{

}

QVariant ClientResourceModel::data(const QModelIndex& index, int role) const
{
  if (role == Qt::DecorationRole && index.column() == 0) {
    if (!index.parent().isValid())
      return m_iconProvider.icon(QFileIconProvider::Drive);
    if (hasChildren(index))
      return m_iconProvider.icon(QFileIconProvider::Folder);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QList<QMimeType> types = m_mimeDb.mimeTypesForFileName(index.data(Qt::DisplayRole).toString());
    foreach( const QMimeType &mt, types) {
      QIcon icon = QIcon::fromTheme(mt.iconName());
      if (!icon.isNull())
        return icon;
      icon = QIcon::fromTheme(mt.genericIconName());
      if (!icon.isNull())
        return icon;
    }
#endif
    return m_iconProvider.icon(QFileIconProvider::File);
  }
  return QSortFilterProxyModel::data(index, role);
}

#include "clientresourcemodel.moc"
