/*
  metaobjectmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_METAOBJECTMODEL_H
#define GAMMARAY_METAOBJECTMODEL_H

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QMetaObject>

#include "metaobjectregistry.h"
#include "probe.h"

namespace GammaRay {
template<typename MetaThing,
         MetaThing(QMetaObject::*MetaAccessor)(int)const,
         int(QMetaObject::*MetaCount)() const,
         int(QMetaObject::*MetaOffset)() const>
class MetaObjectModel : public QAbstractItemModel
{
    Q_DECLARE_TR_FUNCTIONS(GammaRay::MetaObjectModel)
public:
    explicit MetaObjectModel(QObject *parent = nullptr)
        : QAbstractItemModel(parent)
    {
    }

    virtual void setMetaObject(const QMetaObject *metaObject)
    {
        if (m_rowCount) {
            beginRemoveRows(QModelIndex(), 0, m_rowCount - 1);
            m_metaObject = nullptr;
            m_rowCount = 0;
            endRemoveRows();
        } else {
            m_metaObject = nullptr;
        }

        if (!metaObject)
            return;
        if (!Probe::instance()->metaObjectRegistry()->isValid(metaObject))
            return;

        const auto newRowCount = (metaObject->*MetaCount)();
        if (newRowCount) {
            beginInsertRows(QModelIndex(), 0, newRowCount - 1);
            m_metaObject = metaObject;
            m_rowCount = newRowCount;
            endInsertRows();
        } else {
            m_metaObject = metaObject;
        }
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || !m_metaObject
            || index.row() < 0 || index.row() >= rowCount(index.parent()))
            return QVariant();
        if (!Probe::instance()->metaObjectRegistry()->isValid(m_metaObject))
            return QVariant();

        const MetaThing metaThing = (m_metaObject->*MetaAccessor)(index.row());
        if (index.column() == columnCount(index) - 1 && role == Qt::DisplayRole) {
            const QMetaObject *mo = m_metaObject;
            while ((mo->*MetaOffset)() > index.row())
                mo = mo->superClass();
            return mo->className();
        }
        return metaData(index, metaThing, role);
    }

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            if (section == columnCount() - 1)
                return tr("Class");
            return columnHeader(section);
        }
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (!m_metaObject || parent.isValid())
            return 0;
        return m_rowCount;
    }

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override
    {
        if (row >= 0 && row < rowCount(parent) && column >= 0
            && column < columnCount(parent) && !parent.isValid())
            return createIndex(row, column, -1);
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex &child) const override
    {
        Q_UNUSED(child);
        return QModelIndex();
    }

protected:
    virtual QVariant metaData(const QModelIndex &index, const MetaThing &metaThing,
                              int role) const = 0;

    virtual QString columnHeader(int index) const
    {
        Q_UNUSED(index);
        return QString();
    };

    typedef MetaObjectModel<MetaThing, MetaAccessor, MetaCount, MetaOffset> super;

protected:
    // let's assume that meta objects never get destroyed
    // very funny, never heard of QML? ;)
    const QMetaObject *m_metaObject = nullptr;

    // cached row count, so we don't need to dereference a potentially stale m_metaObject
    // in setMetaObject again, as that might have been destroyed meanwhile
    int m_rowCount = 0;
};
}

#endif // GAMMARAY_METAOBJECTMODEL_H
