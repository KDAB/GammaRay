/*
  bindingmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bindingmodel.h"
#include "bindingnode.h"
#include "abstractbindingprovider.h"
#include <common/objectmodel.h>
#include <core/util.h>

#include <QDebug>
#include <private/qobject_p.h>

#include <QMetaProperty>

using namespace GammaRay;

std::vector<std::unique_ptr<AbstractBindingProvider>> BindingModel::s_providers;

//connect to a functor
template <typename Func>
static inline typename std::enable_if<QtPrivate::FunctionPointer<Func>::ArgumentCount == -1, QMetaObject::Connection>::type
        connect(const QObject *sender, int signalIndex, Func slot,
                Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef typename QtPrivate::FunctorReturnType<Func, typename QtPrivate::List<>>::Value SlotReturnType;

    return QObjectPrivate::connect(sender, signalIndex,
                        new QtPrivate::QFunctorSlotObjectWithNoArgs<Func, SlotReturnType>(std::move(slot)), type);
}

void GammaRay::BindingModel::registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider)
{
    s_providers.push_back(std::move(provider));
}

BindingModel::BindingModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_obj(Q_NULLPTR)
{
}

BindingModel::~BindingModel()
{
}

bool BindingModel::setObject(QObject* obj)
{
    if (m_obj == obj)
        return true;

    bool typeMatches = false;
    m_bindings.clear();
    for (auto &&provider: s_providers) {
        if (!provider->canProvideBindingsFor(obj))
            continue;
        else
            typeMatches = true;

        // TODO use removerows/insertrows instead of reset here
        beginResetModel();
        if (m_obj)
            disconnect(m_obj, Q_NULLPTR, this, Q_NULLPTR);

        auto newBindings = provider->findBindingsFor(obj);
        for (std::unique_ptr<BindingNode> &nodeUnique : newBindings) {
            BindingNode *node = nodeUnique.get();
            if (findEquivalent(m_bindings, node).isValid()) {
                continue; // apparantly this is a duplicate.
            }
            int index = m_bindings.size();
            int signalIndex = node->property().notifySignalIndex();
            if (signalIndex != -1) {
                ::connect(obj, signalIndex, [this, index, node]() {
                    refresh(node, createIndex(index, 0, node));
                }, Qt::UniqueConnection);
            }
            findDependenciesFor(node);
            m_bindings.push_back(std::move(nodeUnique));
        }

        endResetModel();
    }
//     std::sort(m_bindings.begin(), m_bindings.end(), [](std::unique_ptr<BindingNode>& node1, std::unique_ptr<BindingNode>& node2){
//         return node1->
//     });
    return typeMatches;
}

bool BindingModel::lessThan(const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b) {
    return a->object() < b->object()
           || (a->object() == b->object() && a->propertyIndex() < b->propertyIndex());
}

void BindingModel::findDependenciesFor(BindingNode* node)
{
    if (node->isBindingLoop())
        return;
    for (auto &&provider : s_providers) {
        for (auto &&dependency : provider->findDependenciesFor(node)) {
            findDependenciesFor(dependency.get());
            node->dependencies().push_back(std::move(dependency));
        }
    }
    std::sort(node->dependencies().begin(), node->dependencies().end(), &BindingModel::lessThan);
}

void BindingModel::refresh(BindingNode *bindingNode, const QModelIndex &index)
{
    if (bindingNode->cachedValue() != bindingNode->readValue()) {
        bindingNode->refreshValue();
        emit dataChanged(createIndex(index.row(), 1, bindingNode), createIndex(index.row(), 1, bindingNode));
    }
    uint oldDepth = bindingNode->depth();

    // Refresh dependencies
    auto &oldDependencies = bindingNode->dependencies();
    std::vector<std::unique_ptr<BindingNode>> newDependencies;
    for (auto &&provider : s_providers) {
        auto deps = provider->findDependenciesFor(bindingNode);
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
//                 auto newNode = new BindingNode(*newIt->get());
//                 newNode->setParent(bindingNode);
//                 findDependenciesFor(newNode);
//                 oldIt = oldDependencies.insert(oldIt, std::unique_ptr<BindingNode>(newNode));
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
//             auto newNode = new BindingNode(*newIt->get());
//             newNode->setParent(bindingNode);
//             findDependenciesFor(newNode);
//             oldDependencies.push_back(std::unique_ptr<BindingNode>(newNode));
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
        emit dataChanged(createIndex(index.row(), 4, bindingNode), createIndex(index.row(), 4, bindingNode));
    }
}

int BindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
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
            case 0: {
                return binding->canonicalName();
            }
            case 1: return binding->cachedValue();
            case 2: return binding->expression();
            case 3: return binding->sourceLocation().displayString();
            case 4: {
                uint depth = binding->depth();
                return depth == std::numeric_limits<uint>::max() ? QStringLiteral("∞") : QString::number(depth);
            }
        }
    } else if (role == ObjectModel::DeclarationLocationRole) {
        return QVariant::fromValue(binding->sourceLocation());
    }

    return QVariant();
}

Qt::ItemFlags BindingModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    BindingNode *binding = static_cast<BindingNode*>(index.internalPointer());
    if (binding && !binding->isActive()) {
        flags &= ~Qt::ItemIsEnabled;
    }
    return flags;
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
            case 0: return tr("Property");
            case 1: return tr("Value");
            case 2: return tr("Expression");
            case 3: return tr("Source");
            case 4: return tr("Depth");
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
