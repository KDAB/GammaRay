/*
  mimetypes.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mimetypes.h"
#include "ui_mimetypes.h"

#include <kde/krecursivefilterproxymodel.h>

#include <QDebug>
#include <QMimeType>
#include <QStandardItemModel>

using namespace GammaRay;

MimeTypes::MimeTypes(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::MimeTypes)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  m_model = new QStandardItemModel(this);
  fillModel();

  QSortFilterProxyModel *proxy = new KRecursiveFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_model);
  ui->mimeTypeView->setModel(proxy);
  ui->mimeTypeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui->mimeTypeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  ui->mimeTypeView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchLine->setProxy(proxy);
}

MimeTypes::~MimeTypes()
{
}

QVector<QStandardItem*> MimeTypes::itemsForType(const QString &mimeTypeName)
{
  if (m_mimeTypeNodes.contains(mimeTypeName)) {
    return m_mimeTypeNodes.value(mimeTypeName);
  }

  makeItemsForType(mimeTypeName);
  return m_mimeTypeNodes.value(mimeTypeName);
}

void MimeTypes::makeItemsForType(const QString &mimeTypeName)
{
  const QMimeType mt = m_db.mimeTypeForName(mimeTypeName);

  if (mt.parentMimeTypes().isEmpty()) {
    const QList<QStandardItem*> row = makeRowForType(mt);
    m_model->appendRow(row);
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

QSet< QString > MimeTypes::normalizedMimeTypeNames(const QStringList &typeNames) const
{
  QSet<QString> res;
  foreach (const QString &typeName, typeNames) {
    const QMimeType mt = m_db.mimeTypeForName(typeName);
    res.insert(mt.name());
  }

  return res;
}

QList<QStandardItem*> MimeTypes::makeRowForType(const QMimeType &mt)
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

void MimeTypes::fillModel()
{
  m_model->clear();
  m_model->setHorizontalHeaderLabels(QStringList() << tr("Name")
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

#include "mimetypes.moc"
