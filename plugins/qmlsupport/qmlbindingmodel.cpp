/*
  qmlbindingmodel.cpp

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

#include "qmlbindingmodel.h"
#include "qmlbindingnode.h"
#include <common/objectmodel.h>
#include <core/util.h>

#include <QDebug>
#include <private/qobject_p.h>

using namespace GammaRay;

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

QmlBindingModel::QmlBindingModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_obj(Q_NULLPTR)
{
    m_providers.push_back(std::unique_ptr<AbstractBindingProvider>(new QmlBindingProvider));
    m_providers.push_back(std::unique_ptr<AbstractBindingProvider>(new QuickImplicitBindingDependencyProvider));
}

QmlBindingModel::~QmlBindingModel()
{
}

bool QmlBindingModel::setObject(QObject* obj)
{
    if (m_obj == obj)
        return true;

    bool typeMatches = false;
    m_bindings.clear();
    for (auto &&provider : m_providers) {
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
            int index = m_bindings.size();
            ::connect(obj, node->property().notifySignalIndex(), [this, index, node]() {
                refresh(node, createIndex(index, 0, node));
            }, Qt::UniqueConnection);
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

void QmlBindingModel::findDependenciesFor(BindingNode* node)
{
    for (auto &&provider : m_providers) {
        for (auto &&dependency : provider->findDependenciesFor(node)) {
            auto childNode = new BindingNode(dependency.object, dependency.propertyIndex, node);
            node->addDependency(std::unique_ptr<BindingNode>(childNode));
            findDependenciesFor(childNode);
        }
    }
}

void QmlBindingModel::refresh(BindingNode *bindingNode, const QModelIndex &index)
{
//     if (bindingNode->value() != newBindingNode->value()) {
        bindingNode->refreshValue();
        emit dataChanged(createIndex(index.row(), 1, bindingNode), createIndex(index.row(), 1, bindingNode));
//     }
//     if (bindingNode->depth() != newBindingNode->depth()) {
//         emit dataChanged(createIndex(index.row(), 4, bindingNode), createIndex(index.row(), 4, bindingNode));
//     }

    // Refresh dependencies
    auto &oldDependencies = bindingNode->dependencies();
    std::vector<AbstractBindingProvider::Dependency> newDependencies;
    for (auto &&provider : m_providers) {
        auto deps = provider->findDependenciesFor(bindingNode);
        newDependencies.insert(newDependencies.end(), deps.cbegin(), deps.cend());
    }
    oldDependencies.reserve(newDependencies.size());
    auto i = oldDependencies.begin();
    auto j = newDependencies.begin();

    while (i != oldDependencies.end() && j != newDependencies.end()) {
        const auto idx = std::distance(oldDependencies.begin(), i);
        if ((*i)->object() == j->object && (*i)->propertyIndex() == j->propertyIndex) { // already known node, no change
            refresh(i->get(), createIndex(idx, 1, i->get()));
            ++i;
            ++j;
        } else if ((*i)->object() < j->object || (*i)->propertyIndex() < j->propertyIndex) { // handle deleted node
            beginRemoveRows(index, idx, idx);
            i = oldDependencies.erase(i);
            endRemoveRows();
        } else { // handle added node
            beginInsertRows(index, idx, idx);
            i = oldDependencies.insert(i, std::unique_ptr<BindingNode>(new BindingNode(j->object, j->propertyIndex, bindingNode)));
            endInsertRows();
            ++i;
            ++j;
        }
    }
    if (i == oldDependencies.end() && j != newDependencies.end()) {
        // Add remaining new items to list and inform the client
        const auto idx = std::distance(oldDependencies.begin(), i);
        const auto count = std::distance(j, newDependencies.end());

        beginInsertRows(index, idx, idx + count - 1);
        while (j != newDependencies.end()) {
            oldDependencies.push_back(std::unique_ptr<BindingNode>(new BindingNode(j->object, j->propertyIndex, bindingNode)));
            ++j;
        }
        endInsertRows();
    } else if (i != oldDependencies.end()) { // Inform the client about the removed rows
        const auto idx = std::distance(oldDependencies.begin(), i);
        const auto count = std::distance(i, oldDependencies.end());

        beginRemoveRows(index, idx, idx + count - 1);
        i = oldDependencies.erase(i, oldDependencies.end());
        endRemoveRows();
    }
}

int QmlBindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int QmlBindingModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return m_bindings.size();
    if (parent.column() != 0)
        return 0;
    return static_cast<BindingNode *>(parent.internalPointer())->dependencies().size();
}

QVariant QmlBindingModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    BindingNode *binding = static_cast<BindingNode*>(index.internalPointer());
    if (!binding)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: {
                return binding->name();
            }
            case 1: return binding->value();
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

Qt::ItemFlags QmlBindingModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    BindingNode *binding = static_cast<BindingNode*>(index.internalPointer());
    if (binding && !binding->isActive()) {
        flags &= ~Qt::ItemIsEnabled;
    }
    return flags;
}

QMap<int, QVariant> QmlBindingModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(ObjectModel::DeclarationLocationRole, data(index, ObjectModel::DeclarationLocationRole));
    return d;
}

QVariant QmlBindingModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QModelIndex GammaRay::QmlBindingModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex QmlBindingModel::findEquivalent(const std::vector<std::unique_ptr<BindingNode>> &container, BindingNode *bindingNode) const
{
    for (size_t i = 0; i < container.size(); i++) {
        if (bindingNode->id() == container[i]->id()) {
            return createIndex(i, 0, container[i].get());
        }
    }
    return QModelIndex();
}

QModelIndex GammaRay::QmlBindingModel::parent(const QModelIndex& child) const
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
