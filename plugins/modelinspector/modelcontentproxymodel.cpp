/*
  modelcontentproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "modelcontentproxymodel.h"

#include <QDebug>
#include <QItemSelectionModel>

using namespace GammaRay;

ModelContentProxyModel::ModelContentProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_selectionModel(nullptr)
{
}

ModelContentProxyModel::~ModelContentProxyModel() = default;

void ModelContentProxyModel::setSourceModel(QAbstractItemModel *model)
{
    setSelectionModel(nullptr);
    QIdentityProxyModel::setSourceModel(model);
}

void ModelContentProxyModel::setSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_ASSERT(!selectionModel || selectionModel->model() == sourceModel());
    if (m_selectionModel == selectionModel)
        return;

    if (m_selectionModel) {
        disconnect(m_selectionModel.data(), &QItemSelectionModel::selectionChanged, this, &ModelContentProxyModel::selectionChanged);
        emitDataChangedForSelection(m_selectionModel->selection());
    }

    m_selectionModel = selectionModel;

    if (m_selectionModel) {
        connect(m_selectionModel.data(), &QItemSelectionModel::selectionChanged, this, &ModelContentProxyModel::selectionChanged);
        emitDataChangedForSelection(m_selectionModel->selection());
    }
}

QVariant ModelContentProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    // Work around crash in QQmlListModel for unknown roles
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 1)
    if (sourceModel() && sourceModel()->inherits("QQmlListModel")) {
        // data on anything not in roleNames() crashes
        if (!sourceModel()->roleNames().contains(role))
            return QVariant();
    }
#endif

    // we override this below, so convey enabled state via a custom role
    // since disabled is less common then enabled, only transfer disabled states
    if (role == DisabledRole) {
        if (QIdentityProxyModel::flags(proxyIndex) & Qt::ItemIsEnabled)
            return QVariant();
        return true;
    }

    if (role == SelectedRole) {
        if (m_selectionModel && m_selectionModel->isSelected(mapToSource(proxyIndex)))
            return true;
        return QVariant();
    }

    // we set the EmptyNameRole to signal this index is unnamed to the delegate
    if (role == IsDisplayStringEmptyRole) {
        return QIdentityProxyModel::data(proxyIndex, Qt::DisplayRole).toString().isNull();
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}

Qt::ItemFlags ModelContentProxyModel::flags(const QModelIndex &index) const
{
    const auto f = QIdentityProxyModel::flags(index);
    if (!index.isValid())
        return f;
    return f | Qt::ItemIsEnabled | Qt::ItemIsSelectable; // always enable items for inspection
}

QMap<int, QVariant> ModelContentProxyModel::itemData(const QModelIndex &index) const
{
    auto d = QIdentityProxyModel::itemData(index);
    auto v = data(index, DisabledRole);
    if (!v.isNull())
        d.insert(DisabledRole, v);
    v = data(index, SelectedRole);
    if (!v.isNull())
        d.insert(SelectedRole, v);
    v = data(index, IsDisplayStringEmptyRole);
    if (!v.isNull())
        d.insert(IsDisplayStringEmptyRole, v);
    return d;
}

void ModelContentProxyModel::emitDataChangedForSelection(const QItemSelection &selection)
{
    for (const auto &range : selection) {
        if (!range.isValid())
            continue;
        emit dataChanged(range.topLeft(), range.bottomRight());
    }
}

void ModelContentProxyModel::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    emitDataChangedForSelection(deselected);
    emitDataChangedForSelection(selected);
}
