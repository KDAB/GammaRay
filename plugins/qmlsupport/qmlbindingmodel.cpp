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
#include <core/util.h>

#define private public
#define protected public
#include <QDebug>

#include <private/qqmlabstractbinding_p.h>
#include <private/qqmlbinding_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>
#include <common/objectmodel.h>

using namespace GammaRay;

QmlBindingModel::QmlBindingModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_obj(Q_NULLPTR)
{
}

QmlBindingModel::~QmlBindingModel()
{
}

void QmlBindingModel::setObject(QObject* obj)
{
    if (m_obj == obj)
        return;

    auto bindings = bindingsFromObject(obj);

    // TODO use removerows/insertrows instead of reset here
    beginResetModel();
    disconnect(m_obj, Q_NULLPTR, this, Q_NULLPTR);

    m_obj = obj;
    m_bindings = std::move(bindings);
    endResetModel();
}

void GammaRay::QmlBindingModel::propertyChanged()
{
    Q_ASSERT(sender() == m_obj);

    for (size_t i = 0; i < m_bindings.size(); ++i) {
        auto binding = m_bindings[i].get();
        if (binding->property().notifySignalIndex() == senderSignalIndex()) {
            auto newBindingNode = new QmlBindingNode(binding->binding());
            refresh(binding, newBindingNode, createIndex(i, 0, binding), true);
            return;
        }
    }
}

void QmlBindingModel::refresh(QmlBindingNode *oldBindingNode, QmlBindingNode *newBindingNode,
                 const QModelIndex &index, bool emitSignals)
{
    if (oldBindingNode->value() != newBindingNode->value()) {
        oldBindingNode->refreshValue();
        emit dataChanged(createIndex(index.row(), 1, oldBindingNode), createIndex(index.row(), 1, oldBindingNode));
    }
    if (oldBindingNode->depth() != newBindingNode->depth()) {
        emit dataChanged(createIndex(index.row(), 4, oldBindingNode), createIndex(index.row(), 4, oldBindingNode));
    }

    // Refresh dependencies
    auto &oldDependencies = oldBindingNode->dependencies();
    oldDependencies.reserve(newBindingNode->dependencies().size());
    auto i = oldDependencies.begin();
    auto j = newBindingNode->dependencies().begin();

    while (i != oldDependencies.end() && j != newBindingNode->dependencies().end()) {
        const auto idx = std::distance(oldDependencies.begin(), i);
        if (**i < **j) { // handle deleted node
            if (emitSignals)
                beginRemoveRows(index, idx, idx);
            i = oldDependencies.erase(i);
            if (emitSignals)
                endRemoveRows();
        } else if (**i > **j) { // handle added node
            if (emitSignals)
                beginInsertRows(index, idx, idx);
            i = oldDependencies.insert(i, std::move(*j));
            if (emitSignals)
                endInsertRows();
            ++i;
            ++j;
        } else { // already known node, no change
            refresh(i->get(), j->get(), createIndex(idx, 1, i->get()), emitSignals);
            ++i;
            ++j;
        }
    }
    if (i == oldDependencies.end() && j != newBindingNode->dependencies().end()) {
        // Add remaining new items to list and inform the client
        const auto idx = std::distance(oldDependencies.begin(), i);
        const auto count = std::distance(j, newBindingNode->dependencies().end());

        if (emitSignals)
            beginInsertRows(index, idx, idx + count - 1);
        while (j != newBindingNode->dependencies().end()) {
            (*j)->setParent(static_cast<QmlBindingNode *>(index.internalPointer()));
            oldDependencies.push_back(std::move(*j));
            ++j;
        }
        if (emitSignals)
            endInsertRows();
    } else if (i != oldDependencies.end()) { // Inform the client about the removed rows
        const auto idx = std::distance(oldDependencies.begin(), i);
        const auto count = std::distance(i, oldDependencies.end());

        if (emitSignals)
            beginRemoveRows(index, idx, idx + count - 1);
        i = oldDependencies.erase(i, oldDependencies.end());
        if (emitSignals)
            endRemoveRows();
    }
}

std::vector<std::unique_ptr<QmlBindingNode>> QmlBindingModel::bindingsFromObject(QObject* obj)
{
    std::vector<std::unique_ptr<QmlBindingNode>> bindings;
    if (!obj)
        return bindings;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
    if (!data)
        return bindings;

    auto b = data->bindings;
    while (b) {
        if (auto qmlBinding = dynamic_cast<QQmlBinding*>(b)) {
            QmlBindingNode *node;

            node = new QmlBindingNode(qmlBinding);
            QMetaObject::connect(obj, node->property().notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"), Qt::UniqueConnection);

            bindings.push_back(std::unique_ptr<QmlBindingNode>(node));
        } else {
            qDebug() << "Ohhh...";
        }
        b = b->nextBinding();
    }
#endif
    return bindings;
}

int QmlBindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int QmlBindingModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<QmlBindingNode *>(parent.internalPointer())->dependencies().size();
    return m_bindings.size();
}

QVariant QmlBindingModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_obj)
        return QVariant();

    QmlBindingNode *binding = static_cast<QmlBindingNode*>(index.internalPointer());
    if (!binding)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: {
                return binding->id().isEmpty() ? binding->property().name() : QString(binding->id() + '.' + binding->property().name());
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
    QmlBindingNode *binding = static_cast<QmlBindingNode*>(index.internalPointer());
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
        index = createIndex(row, column, static_cast<QmlBindingNode *>(parent.internalPointer())->dependencies()[row].get());
    } else {
        index = createIndex(row, column, m_bindings[row].get());
    }
    return index;
}

QModelIndex GammaRay::QmlBindingModel::parent(const QModelIndex& child) const
{
    QmlBindingNode *parent = static_cast<QmlBindingNode *>(child.internalPointer())->parent();
    if (!parent) {
        return QModelIndex();
    }
    QmlBindingNode *grandparent = parent->parent();
    if (!grandparent) {
        for (size_t i = 0; i < m_bindings.size(); i++) {
            if (parent == m_bindings[i].get()) {
                return createIndex(i, child.column(), m_bindings[i].get());
            }
        }
    }
    for (size_t i = 0; i < grandparent->dependencies().size(); i++) {
        if (parent == grandparent->dependencies()[i].get()) {
            return createIndex(i, child.column(), grandparent->dependencies()[i].get());
        }
    }
    return QModelIndex();
}
