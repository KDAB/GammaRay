/*
  attributemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
