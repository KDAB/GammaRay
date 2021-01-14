/*
  bindingmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

// Own
#include "bindingmodel.h"

#include <common/objectmodel.h>
#include <common/problem.h>
#include <core/problemcollector.h>
#include <core/objectdataprovider.h>
#include <core/abstractbindingprovider.h>
#include <core/bindingnode.h>
#include <core/util.h>

// Qt
#include <QDebug>
#include <QMetaProperty>

using namespace GammaRay;

BindingModel::BindingModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_obj(nullptr)
    , m_bindings(nullptr)
{
}

BindingModel::~BindingModel() = default;

void BindingModel::aboutToClear()
{
    beginResetModel();
}

void BindingModel::cleared()
{
    m_obj = nullptr;
    endResetModel();
}

void BindingModel::setObject(QObject* obj, std::vector<std::unique_ptr<BindingNode>> &bindings)
{
    if (m_obj == obj)
        return;

    // TODO use removerows/insertrows instead of reset here
    beginResetModel();
    m_bindings = &bindings;
    m_obj = obj;
    endResetModel();
}

void GammaRay::BindingModel::refresh(int row, std::vector<std::unique_ptr<BindingNode>> &&newDependencies)
{
    Q_ASSERT(m_bindings);
    refresh((*m_bindings)[row].get(), std::move(newDependencies), createIndex(row, 0, (*m_bindings)[row].get()));
}

bool BindingModel::lessThan(const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b) {
    return a->object() < b->object()
           || (a->object() == b->object() && a->propertyIndex() < b->propertyIndex());
}

void BindingModel::refresh(BindingNode *oldBindingNode, std::vector<std::unique_ptr<BindingNode>> &&newDependencies, const QModelIndex &index)
{
    if (oldBindingNode->cachedValue() != oldBindingNode->readValue()) {
        oldBindingNode->refreshValue();
        emit dataChanged(createIndex(index.row(), ValueColumn, oldBindingNode), createIndex(index.row(), ValueColumn, oldBindingNode));
    }
    uint oldDepth = oldBindingNode->depth();

    // Refresh dependencies
    auto &oldDependencies = oldBindingNode->dependencies();
    std::sort(newDependencies.begin(), newDependencies.end(), &BindingModel::lessThan);
    oldDependencies.reserve(newDependencies.size());
    auto oldIt = oldDependencies.begin();
    auto newIt = newDependencies.begin();

    while (oldIt != oldDependencies.end() && newIt != newDependencies.end()) {
        const auto idx = std::distance(oldDependencies.begin(), oldIt);
        if (lessThan(*oldIt, *newIt)) { // handle deleted node
            const auto firstToRemove = oldIt;
            while (oldIt != oldDependencies.end() && lessThan(*oldIt, *newIt)) { ++oldIt; } // if more than one was removed, find all
            const auto count = std::distance(firstToRemove, oldIt);
            beginRemoveRows(index, idx, idx + count - 1);
            oldIt = oldDependencies.erase(firstToRemove, oldIt);
            endRemoveRows();
        } else if (lessThan(*newIt, *oldIt)) { // handle added node
            int count = 0;
            for (auto addIt = newIt; addIt != newDependencies.end() && lessThan(*addIt, *oldIt); ++addIt) {
                ++count; // count, how many additions we have
            }
            beginInsertRows(index, idx, idx + count - 1);
            for (int i = 0; i < count; ++i) {
                (*newIt)->setParent(oldBindingNode);
                oldIt = oldDependencies.insert(oldIt, std::move(*newIt));
                ++oldIt;
                ++newIt;
            }
            endInsertRows();
        } else { // already known node, no change
            refresh(oldIt->get(), std::move(newIt->get()->dependencies()), createIndex(idx, 0, oldIt->get()));
            ++oldIt;
            ++newIt;
        }
    }
    if (oldIt == oldDependencies.end() && newIt != newDependencies.end()) {
        // Add remaining new items to list and inform the client
        const auto idx = std::distance(oldDependencies.begin(), oldIt);
        const auto count = std::distance(newIt, newDependencies.end());

        beginInsertRows(index, idx, idx + count - 1);
        while (newIt != newDependencies.end()) {
            (*newIt)->setParent(oldBindingNode);
            oldDependencies.push_back(std::move(*newIt));
            ++newIt;
        }
        endInsertRows();
    } else if (oldIt != oldDependencies.end()) { // Inform the client about the removed rows
        const auto idx = std::distance(oldDependencies.begin(), oldIt);
        const auto count = std::distance(oldIt, oldDependencies.end());

        beginRemoveRows(index, idx, idx + count - 1);
        oldIt = oldDependencies.erase(oldIt, oldDependencies.end());
        endRemoveRows();
    }

    if (oldBindingNode->depth() != oldDepth) {
        emit dataChanged(createIndex(index.row(), DepthColumn, oldBindingNode), createIndex(index.row(), DepthColumn, oldBindingNode));
    }
}

int BindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int BindingModel::rowCount(const QModelIndex& parent) const
{
    if (!m_bindings)
        return 0;
    if (!parent.isValid())
        return m_bindings->size();
    if (parent.column() != 0)
        return 0;
    return static_cast<BindingNode *>(parent.internalPointer())->dependencies().size();
}

QVariant BindingModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    BindingNode *binding = static_cast<BindingNode*>(index.internalPointer());
    if (!binding)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case NameColumn: {
                return binding->canonicalName();
            }
            case ValueColumn: return binding->cachedValue();
            case LocationColumn: return binding->sourceLocation().displayString();
            case DepthColumn: {
                uint depth = binding->depth();
                return depth == std::numeric_limits<uint>::max() ? QStringLiteral("\u221E") : QString::number(depth); // Unicode infinity sign
            }
        }
    } else if (role == ObjectModel::DeclarationLocationRole) {
        return QVariant::fromValue(binding->sourceLocation());
    }

    return QVariant();
}

QMap<int, QVariant> BindingModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(ObjectModel::DeclarationLocationRole, data(index, ObjectModel::DeclarationLocationRole));
    return d;
}

QVariant BindingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case NameColumn: return tr("Property");
            case ValueColumn: return tr("Value");
            case LocationColumn: return tr("Source");
            case DepthColumn: return tr("Depth");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex GammaRay::BindingModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_bindings || !hasIndex(row, column, parent)) {
        return {};
    }
    QModelIndex index;
    if (parent.isValid()) {
        index = createIndex(row, column, static_cast<BindingNode *>(parent.internalPointer())->dependencies()[row].get());
    } else {
        index = createIndex(row, column, (*m_bindings)[row].get());
    }
    return index;
}

QModelIndex BindingModel::findEquivalent(const std::vector<std::unique_ptr<BindingNode>> &container, BindingNode *bindingNode) const
{
    for (size_t i = 0; i < container.size(); i++) {
        if (bindingNode->object() == container[i]->object() && bindingNode->propertyIndex() == container[i]->propertyIndex()) {
            return createIndex(i, 0, container[i].get());
        }
    }
    return {};
}

QModelIndex GammaRay::BindingModel::parent(const QModelIndex& child) const
{
    if (!m_bindings || !child.isValid())
        return {};

    BindingNode *parent = static_cast<BindingNode *>(child.internalPointer())->parent();
    if (!parent)
        return QModelIndex();

    BindingNode *grandparent = parent->parent();

    if (!grandparent)
        return findEquivalent(*m_bindings, parent);

    return findEquivalent(grandparent->dependencies(), parent);
}
