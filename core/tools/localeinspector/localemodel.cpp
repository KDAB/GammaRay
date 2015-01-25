/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "localemodel.h"
#include "localedataaccessor.h"

#include <QDebug>
#include <QLocale>

using namespace GammaRay;

LocaleModel::LocaleModel(LocaleDataAccessorRegistry *registry, QObject *parent)
  : QAbstractTableModel(parent), m_registry(registry)
{
  init();
  connect(registry, SIGNAL(accessorsChanged()), SLOT(reinit()));
}

int LocaleModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_localeData.size();
}

QVariant LocaleModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() ||
      index.row() >= m_locales.size() ||
      index.column() >= m_localeData.size()) {
    return QVariant();
  }

  const QLocale l = m_locales.at(index.row());
  return m_localeData.at(index.column())->data(l, role);
}

QVariant LocaleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  if (orientation == Qt::Vertical) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  LocaleDataAccessor *d = m_localeData.at(section);
  return d->accessorName();
}

void LocaleModel::init()
{
  m_localeData = m_registry->enabledAccessors();

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
  m_locales =
    QLocale::matchingLocales(QLocale::AnyLanguage,
                             QLocale::AnyScript, QLocale::AnyCountry).toVector();
#else
  m_locales.clear();
  QLocale::Language l = QLocale::C;
  while (QLocale::languageToString(l) != QLatin1String("Unknown"))
  {
    QList<QLocale::Country> countries = QLocale::countriesForLanguage(l);
    Q_FOREACH (const QLocale::Country &c, countries) {
      m_locales.append(QLocale(l, c));
    }
    l = (QLocale::Language)((int)(l) + 1);
  }
#endif
}

void LocaleModel::reinit()
{
  beginResetModel();
  init();
  endResetModel();
}

int LocaleModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_locales.size();
}

