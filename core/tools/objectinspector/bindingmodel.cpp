/*
  bindingmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <core/abstractbindingprovider.h>
#include <core/bindingnode.h>
#include <core/util.h>

// Qt
#include <QDebug>
#include <QMetaProperty>

using namespace GammaRay;

std::vector<std::unique_ptr<AbstractBindingProvider>> BindingModel::s_providers;

void GammaRay::BindingModel::registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider)
{
    s_providers.push_back(std::move(provider));
}

BindingModel::BindingModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_obj(nullptr)
{
}

BindingModel::~BindingModel()
{
}

void BindingModel::clear()
{
    beginResetModel();
    m_bindings.clear();
    if (m_obj)
        disconnect(m_obj, nullptr, this, nullptr);
    m_obj = nullptr;
    endResetModel();

}

bool BindingModel::setObject(QObject* obj)
{
    if (m_obj == obj)
        return obj;

    // TODO use removerows/insertrows instead of reset here
    beginResetModel();
    if (m_obj)
        disconnect(m_obj, nullptr, this, nullptr);
    bool typeMatches = false;
    m_bindings.clear();
    if (obj) {
        for (auto providerIt = s_providers.begin(); providerIt != s_providers.cend(); ++providerIt) {
            auto &&provider = *providerIt;
            if (!provider->canProvideBindingsFor(obj))
                continue;
            else
                typeMatches = true;

            auto newBindings = provider->findBindingsFor(obj);
            for (auto nodeIt = newBindings.begin(); nodeIt != newBindings.end(); ++nodeIt) {
                BindingNode *node = nodeIt->get();
                if (findEquivalent(m_bindings, node).isValid()) {
                    continue; // apparantly this is a duplicate.
                }
                int signalIndex = node->property().notifySignalIndex();
                if (signalIndex != -1) {
                    QMetaObject::connect(obj, signalIndex, this, metaObject()->indexOfMethod("propertyChanged()"), Qt::UniqueConnection);
                }
                findDependenciesFor(node);
                m_bindings.push_back(std::move(*nodeIt));
            }

        }
        connect(obj, SIGNAL(destroyed()), this, SLOT(clear()));
    }
    m_obj = obj;
    endResetModel();
    return typeMatches;
}

void BindingModel::propertyChanged()
{
    Q_ASSERT(sender() == m_obj);

    for (size_t i = 0; i < m_bindings.size(); ++i) {
        const auto &bindingNode = m_bindings[i];
        if (bindingNode->property().notifySignalIndex() == senderSignalIndex()) {
            refresh(bindingNode.get(), createIndex(i, 0, bindingNode.get()));
            // There can be more than one property with the same notify signal,
            // so no break here...
        }
    }
}

bool BindingModel::lessThan(const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b) {
    return a->object() < b->object()
           || (a->object() == b->object() && a->propertyIndex() < b->propertyIndex());
}

void BindingModel::findDependenciesFor(BindingNode* node)
{
    if (node->isBindingLoop())
        return;
    for (auto providerIt = s_providers.cbegin(); providerIt != s_providers.cend(); ++providerIt) {
        auto &&provider = *providerIt;
        auto dependencies = provider->findDependenciesFor(node);
        for (auto dependencyIt = dependencies.begin(); dependencyIt != dependencies.end(); ++dependencyIt) {
            findDependenciesFor(dependencyIt->get());
            node->dependencies().push_back(std::move(*dependencyIt));
        }
    }
    std::sort(node->dependencies().begin(), node->dependencies().end(), &BindingModel::lessThan);
}

void BindingModel::refresh(BindingNode *bindingNode, const QModelIndex &index)
{
    if (bindingNode->cachedValue() != bindingNode->readValue()) {
        bindingNode->refreshValue();
        emit dataChanged(createIndex(index.row(), ValueColumn, bindingNode), createIndex(index.row(), ValueColumn, bindingNode));
    }
    uint oldDepth = bindingNode->depth();

    // Refresh dependencies
    auto &oldDependencies = bindingNode->dependencies();
    std::vector<std::unique_ptr<BindingNode>> newDependencies;
    for (auto providerIt = s_providers.begin(); providerIt != s_providers.cend(); ++providerIt) {
        auto deps = (*providerIt)->findDependenciesFor(bindingNode);
        newDependencies.insert(newDependencies.end(), std::make_move_iterator(deps.begin()), std::make_move_iterator(deps.end()));
    }
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
                (*newIt)->setParent(bindingNode);
                findDependenciesFor(newIt->get());
                oldIt = oldDependencies.insert(oldIt, std::move(*newIt));
                ++oldIt;
                ++newIt;
            }
            endInsertRows();
        } else { // already known node, no change
            refresh(oldIt->get(), createIndex(idx, 0, oldIt->get()));
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
            (*newIt)->setParent(bindingNode);
            findDependenciesFor(newIt->get());
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

    if (bindingNode->depth() != oldDepth) {
        emit dataChanged(createIndex(index.row(), DepthColumn, bindingNode), createIndex(index.row(), DepthColumn, bindingNode));
    }
}

int BindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int BindingModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return m_bindings.size();
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
                return depth == std::numeric_limits<uint>::max() ? QStringLiteral("∞") : QString::number(depth);
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
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    QModelIndex index;
    if (parent.isValid()) {
        index = createIndex(row, column, static_cast<BindingNode *>(parent.internalPointer())->dependencies()[row].get());
    } else {
        index = createIndex(row, column, m_bindings[row].get());
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
    return QModelIndex();
}

QModelIndex GammaRay::BindingModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    BindingNode *parent = static_cast<BindingNode *>(child.internalPointer())->parent();
    if (!parent)
        return QModelIndex();

    BindingNode *grandparent = parent->parent();

    if (!grandparent)
        return findEquivalent(m_bindings, parent);

    return findEquivalent(grandparent->dependencies(), parent);
}
