/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEMODEL_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QLocale>

namespace GammaRay {
class LocaleDataAccessorRegistry;
struct LocaleDataAccessor;

class LocaleModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LocaleModel(LocaleDataAccessorRegistry *registry, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    void init();

private slots:
    void accessorAdded();
    void accessorRemoved(int idx);

private:
    QVector<QLocale> m_locales;
    QVector<LocaleDataAccessor *> m_localeData;
    LocaleDataAccessorRegistry *m_registry;
};
}

#endif
