/*
  metaobjectmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_METAOBJECTMODEL_H
#define GAMMARAY_METAOBJECTMODEL_H

#include <QAbstractItemModel>
#include <QMetaObject>

namespace GammaRay {

template <typename MetaThing,
          MetaThing (QMetaObject::*MetaAccessor)(int) const,
          int (QMetaObject::*MetaCount)() const,
          int (QMetaObject::*MetaOffset)() const>
class MetaObjectModel : public QAbstractItemModel
{
  public:
    explicit MetaObjectModel(QObject *parent = 0)
      : QAbstractItemModel(parent), m_metaObject(0)
    {
    }

    virtual void setMetaObject(const QMetaObject *metaObject)
    {
      m_metaObject = metaObject;
      reset();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
      if (!index.isValid() || !m_metaObject ||
          index.row() < 0 || index.row() >= rowCount(index.parent())) {
        return QVariant();
      }

      const MetaThing metaThing = (m_metaObject->*MetaAccessor)(index.row());
      if (index.column() == columnCount(index) - 1 && role == Qt::DisplayRole) {
        const QMetaObject *mo = m_metaObject;
        while ((mo->*MetaOffset)() > index.row()) {
          mo = mo->superClass();
        }
        return mo->className();
      }
      return metaData(index, metaThing, role);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
      if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == columnCount() - 1) {
          return tr("Class");
        }
        return columnHeader(section);
      }
      return QAbstractItemModel::headerData(section, orientation, role);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
      if (!m_metaObject || parent.isValid()) {
        return 0;
      }
      return (m_metaObject->*MetaCount)();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
      if (row >= 0 && row < rowCount(parent) && column >= 0 &&
          column < columnCount(parent) && !parent.isValid()) {
        return createIndex(row, column, -1);
      }
      return QModelIndex();
    }

    QModelIndex parent(const QModelIndex &child) const
    {
      Q_UNUSED(child);
      return QModelIndex();
    }

  protected:
    virtual QVariant metaData(const QModelIndex &index,
                              const MetaThing &metaThing, int role) const = 0;

    virtual QString columnHeader(int index) const = 0;

    typedef MetaObjectModel<MetaThing, MetaAccessor, MetaCount, MetaOffset> super;

  protected:
    // let's assume that meta objects never get destroyed
    const QMetaObject *m_metaObject;
};

}

#endif // GAMMARAY_METAOBJECTMODEL_H
