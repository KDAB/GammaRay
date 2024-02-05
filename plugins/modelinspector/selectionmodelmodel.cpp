/*
  selectionmodelmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "selectionmodelmodel.h"

#include <core/objectdataprovider.h>

#include <QItemSelectionModel>

#include <algorithm>

using namespace GammaRay;

SelectionModelModel::SelectionModelModel(QObject *parent)
    : ObjectModelBase<QAbstractTableModel>(parent)
    , m_model(nullptr)
{
}

SelectionModelModel::~SelectionModelModel() = default;

void SelectionModelModel::objectCreated(QObject *obj)
{
    Q_ASSERT(obj);
    auto model = qobject_cast<QItemSelectionModel *>(obj);
    if (!model)
        return;

    auto it = std::lower_bound(m_selectionModels.begin(), m_selectionModels.end(), model);
    if (it != m_selectionModels.end() && *it == model)
        return;
    m_selectionModels.insert(it, model);
    connect(model, &QItemSelectionModel::selectionChanged, this, &SelectionModelModel::selectionChanged);
    connect(model, &QItemSelectionModel::modelChanged, this, &SelectionModelModel::sourceModelChanged);

    if (!m_model || model->model() != m_model)
        return;

    it = std::lower_bound(m_currentSelectionModels.begin(), m_currentSelectionModels.end(), model);
    const auto row = std::distance(m_currentSelectionModels.begin(), it);
    beginInsertRows(QModelIndex(), row, row);
    m_currentSelectionModels.insert(it, model);
    endInsertRows();
}

void SelectionModelModel::objectDestroyed(QObject *obj)
{
    Q_ASSERT(obj);

    // do not dereference!
    QItemSelectionModel *unsafeModelPtr = nullptr;
    memcpy(&unsafeModelPtr, &obj, sizeof(unsafeModelPtr));

    auto it = std::lower_bound(m_selectionModels.begin(), m_selectionModels.end(), unsafeModelPtr);
    if (it == m_selectionModels.end() || *it != unsafeModelPtr)
        return;
    m_selectionModels.erase(it);

    it = std::lower_bound(m_currentSelectionModels.begin(), m_currentSelectionModels.end(), unsafeModelPtr);
    if (it == m_currentSelectionModels.end() || *it != unsafeModelPtr)
        return;
    const auto row = std::distance(m_currentSelectionModels.begin(), it);
    beginRemoveRows(QModelIndex(), row, row);
    m_currentSelectionModels.erase(it);
    endRemoveRows();
}

void SelectionModelModel::sourceModelChanged()
{
    auto model = qobject_cast<QItemSelectionModel *>(sender());
    Q_ASSERT(model);

    auto it = std::lower_bound(m_currentSelectionModels.begin(), m_currentSelectionModels.end(), model);
    if (it != m_currentSelectionModels.end() && *it == model && model->model() != m_model && m_model) {
        const auto row = std::distance(m_currentSelectionModels.begin(), it);
        beginRemoveRows(QModelIndex(), row, row);
        m_currentSelectionModels.erase(it);
        endRemoveRows();
    }

    if (model->model() == m_model && m_model) {
        auto it = std::lower_bound(m_currentSelectionModels.begin(), m_currentSelectionModels.end(), model);
        if (it != m_currentSelectionModels.end() && *it == model)
            return;
        const auto row = std::distance(m_currentSelectionModels.begin(), it);
        beginInsertRows(QModelIndex(), row, row);
        m_currentSelectionModels.insert(it, model);
        endInsertRows();
    }
}

void SelectionModelModel::selectionChanged()
{
    auto model = qobject_cast<QItemSelectionModel *>(sender());
    Q_ASSERT(model);

    if (model->model() != m_model)
        return;

    const auto it = std::lower_bound(m_currentSelectionModels.constBegin(), m_currentSelectionModels.constEnd(), model);
    Q_ASSERT(it != m_currentSelectionModels.constEnd() && *it == model);
    const auto row = std::distance(m_currentSelectionModels.constBegin(), it);
    emit dataChanged(index(row, 1), index(row, 3));
}

void SelectionModelModel::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
        return;

    if (!m_currentSelectionModels.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_currentSelectionModels.size() - 1);
        m_currentSelectionModels.clear();
        endRemoveRows();
    }

    m_model = model;
    QVector<QItemSelectionModel *> models;
    std::copy_if(m_selectionModels.constBegin(), m_selectionModels.constEnd(), std::back_inserter(models), [this](QItemSelectionModel *model) {
        return model->model() == m_model;
    });

    if (models.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, models.size() - 1);
    m_currentSelectionModels = std::move(models);
    endInsertRows();
}

int SelectionModelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int SelectionModelModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_currentSelectionModels.size();
}

QVariant SelectionModelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto model = m_currentSelectionModels.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 1:
            return model->selectedIndexes().size();
        case 2:
            return model->selectedRows().size();
        case 3:
            return model->selectedColumns().size();
        case 4:
            return ObjectDataProvider::typeName(model);
        }
    }

    return dataForObject(model, index, role);
}

QVariant SelectionModelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Object");
        case 1:
            return tr("#Items");
        case 2:
            return tr("#Rows");
        case 3:
            return tr("#Columns");
        case 4:
            return tr("Type");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
