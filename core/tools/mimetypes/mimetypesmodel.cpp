#include "mimetypesmodel.h"

#include <QDebug>
#include <QMimeType>

using namespace GammaRay;

MimeTypesModel::MimeTypesModel(QObject* parent):
  QStandardItemModel(parent)
{
  fillModel();
}

MimeTypesModel::~MimeTypesModel()
{
}

QVariant MimeTypesModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && role == Qt::DecorationRole && index.column() == 3) {
    QStandardItem *item = itemFromIndex(index);
    if (!item)
      return QVariant();

    // on-demand lookup of icons, too slow to do that all in one go
    const QVariant v = item->data(Qt::DecorationRole);
    if (v.isNull()) {
      const QString iconName = item->data(IconNameRole).toString();
      const QString genericIconName = item->data(GenericIconNameRole).toString();
      if (iconName.isEmpty() && genericIconName.isEmpty())
        return QVariant();

      QIcon icon = QIcon::fromTheme(item->data(IconNameRole).toString());
      if (icon.isNull())
        icon = QIcon::fromTheme(item->data(GenericIconNameRole).toString());
      const_cast<MimeTypesModel*>(this)->blockSignals(true);
      item->setIcon(icon);
      item->setData(QVariant(), IconNameRole); // reset to avoid trying to look up an icon multiple times
      item->setData(QVariant(), GenericIconNameRole);
      const_cast<MimeTypesModel*>(this)->blockSignals(false);
      return icon;
    } else {
      return v;
    }
  }
  return QStandardItemModel::data(index, role);
}

QVector<QStandardItem*> MimeTypesModel::itemsForType(const QString &mimeTypeName)
{
  if (m_mimeTypeNodes.contains(mimeTypeName)) {
    return m_mimeTypeNodes.value(mimeTypeName);
  }

  makeItemsForType(mimeTypeName);
  return m_mimeTypeNodes.value(mimeTypeName);
}

void MimeTypesModel::makeItemsForType(const QString &mimeTypeName)
{
  const QMimeType mt = m_db.mimeTypeForName(mimeTypeName);

  if (mt.parentMimeTypes().isEmpty()) {
    const QList<QStandardItem*> row = makeRowForType(mt);
    appendRow(row);
    m_mimeTypeNodes[mt.name()].push_back(row.first());
  } else {
    // parentMimeTypes contains duplicates and aliases
    const QSet<QString> parentMimeTypeNames = normalizedMimeTypeNames(mt.parentMimeTypes());
    foreach (const QString &parentTypeName, parentMimeTypeNames) {
      foreach (QStandardItem *parentItem, itemsForType(parentTypeName)) {
        const QList<QStandardItem*> row = makeRowForType(mt);
        parentItem->appendRow(row);
        m_mimeTypeNodes[mt.name()].push_back(row.first());
      }
    }
  }
}

QSet< QString > MimeTypesModel::normalizedMimeTypeNames(const QStringList &typeNames) const
{
  QSet<QString> res;
  foreach (const QString &typeName, typeNames) {
    const QMimeType mt = m_db.mimeTypeForName(typeName);
    res.insert(mt.name());
  }

  return res;
}

QList<QStandardItem*> MimeTypesModel::makeRowForType(const QMimeType &mt)
{
  QList<QStandardItem*> row;
  QStandardItem *item = new QStandardItem;
  item->setText(mt.name());
  row.push_back(item);

  item = new QStandardItem;
  item->setText(mt.comment());
  row.push_back(item);

  item = new QStandardItem;
  item->setText(mt.globPatterns().join(QLatin1String(", ")));
  row.push_back(item);

  item = new QStandardItem;
  item->setText(mt.iconName() + QLatin1String(" / ") + mt.genericIconName());
  item->setData(mt.iconName(), IconNameRole);
  item->setData(mt.genericIconName(), GenericIconNameRole);
  row.push_back(item);

  item = new QStandardItem;
  QString s = mt.suffixes().join(QLatin1String(", "));
  if (!mt.preferredSuffix().isEmpty() && mt.suffixes().size() > 1) {
    s += QLatin1String(" (") + mt.preferredSuffix() + QLatin1Char(')');
  }
  item->setText(s);
  row.push_back(item);

  item = new QStandardItem;
  item->setText(mt.aliases().join(QLatin1String(", ")));
  row.push_back(item);

  return row;
}

void MimeTypesModel::fillModel()
{
  clear();
  setHorizontalHeaderLabels(QStringList() << tr("Name")
                                          << tr("Comment")
                                          << tr("Glob Patterns")
                                          << tr("Icons")
                                          << tr("Suffixes")
                                          << tr("Aliases"));

  foreach (const QMimeType &mt, m_db.allMimeTypes()) {
    if (!m_mimeTypeNodes.contains(mt.name())) {
      makeItemsForType(mt.name());
    }
  }

  m_mimeTypeNodes.clear();
}

#include "mimetypesmodel.moc"
