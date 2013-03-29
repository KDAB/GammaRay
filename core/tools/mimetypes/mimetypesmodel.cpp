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
  // TODO load icon on demand
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
  const QIcon icon = QIcon::fromTheme(mt.iconName());
  if (icon.isNull()) {
    item->setIcon(QIcon::fromTheme(mt.genericIconName()));
  } else {
    item->setIcon(icon);
  }
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
