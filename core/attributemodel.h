/*
  attributemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ATTRIBUTEMODEL_H
#define GAMMARAY_ATTRIBUTEMODEL_H

#include "gammaray_core_export.h"

#include <QAbstractTableModel>
#include <QMetaEnum>

namespace GammaRay {
class GAMMARAY_CORE_EXPORT AbstractAttributeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AbstractAttributeModel(QObject *parent = nullptr);
    ~AbstractAttributeModel() override;

    void setAttributeType(const char *name);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

protected:
    virtual bool testAttribute(int attr) const = 0;
    virtual void setAttribute(int attr, bool on) = 0;

private:
    QMetaEnum m_attrs;
};

template<typename Class, typename Enum>
class AttributeModel : public AbstractAttributeModel
{
public:
    explicit AttributeModel(QObject *parent = nullptr)
        : AbstractAttributeModel(parent)
    {
    }

    ~AttributeModel() override = default;

    void setObject(Class *obj)
    {
        if (m_obj == obj)
            return;

        m_obj = obj;
        // cppcheck-suppress nullPointer
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }

protected:
    bool testAttribute(int attr) const override
    {
        if (!m_obj)
            return false;
        return m_obj->testAttribute(static_cast<Enum>(attr));
    }

    void setAttribute(int attr, bool on) override
    {
        if (!m_obj)
            return;
        m_obj->setAttribute(static_cast<Enum>(attr), on);
    }

private:
    Class *m_obj = nullptr;
};
}

#endif // GAMMARAY_ATTRIBUTEMODEL_H
