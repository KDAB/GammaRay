/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEACCESSORMODEL_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEACCESSORMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <common/modelroles.h>

namespace GammaRay {

class LocaleDataAccessorRegistry;
struct LocaleDataAccessor;

class LocaleAccessorModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_ENUMS(CustomRoles)
  public:
    enum CustomRoles {
      AccessorRole = UserRole + 1
    };

    explicit LocaleAccessorModel(LocaleDataAccessorRegistry *registry, QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

  private:
    QVector<LocaleDataAccessor*> enabledAccessors;
    LocaleDataAccessorRegistry *m_registry;
};

}

#endif
