/*
  aggregatedpropertymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_AGGREGATEDPROPERTYMODEL_H
#define GAMMARAY_AGGREGATEDPROPERTYMODEL_H

#include "gammaray_core_export.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QVector>

#include <unordered_map>

namespace GammaRay {
class PropertyAdaptor;
class PropertyData;
class ObjectInstance;

/** Generic property model. */
class GAMMARAY_CORE_EXPORT AggregatedPropertyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AggregatedPropertyModel(QObject *parent = nullptr);
    ~AggregatedPropertyModel() override;

    void setObject(const ObjectInstance &oi);
    void setReadOnly(bool readOnly);

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

private:
    void clear();
    PropertyAdaptor *adaptorForIndex(const QModelIndex &index) const;
    void addPropertyAdaptor(PropertyAdaptor *adaptor) const;
    static QVariant data(PropertyAdaptor *adaptor, const PropertyData &d, int column, int role);
    static bool hasLoop(PropertyAdaptor *adaptor, const QVariant &v);
    void reloadSubTree(PropertyAdaptor *parentAdaptor, int index);
    bool isParentEditable(PropertyAdaptor *adaptor) const;
    void propagateWrite(PropertyAdaptor *adaptor);

private slots:
    void propertyChanged(int first, int last);
    void propertyAdded(int first, int last);
    void propertyRemoved(int first, int last);
    void objectInvalidated();
    void objectInvalidated(GammaRay::PropertyAdaptor *adaptor);

private:
    PropertyAdaptor *m_rootAdaptor = nullptr;
    mutable std::unordered_map<PropertyAdaptor *, QVector<PropertyAdaptor *>> m_parentChildrenMap;
    bool m_inhibitAdaptorCreation = false;
    bool m_readOnly = false;
};
}

#endif // GAMMARAY_AGGREGATEDPROPERTYMODEL_H
