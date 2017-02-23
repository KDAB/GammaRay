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

    const auto bindings = bindingsFromObject(obj);

    // TODO use removerows/insertrows instead of reset here
    beginResetModel();
    disconnect(m_obj, 0, this, 0);

    m_obj = obj;
    m_bindings = bindings;
    endResetModel();
}

void GammaRay::QmlBindingModel::propertyChanged()
{
    Q_ASSERT(sender() == m_obj);

    for (int i = 0; i < m_bindings.size(); i++) {
        auto binding = m_bindings[i];
        if (binding->property().notifySignalIndex() == senderSignalIndex()) {
            binding->refresh();
            QModelIndex modelIndex1 = index(i, 1, QModelIndex());
            QModelIndex modelIndex5 = index(i, 5, QModelIndex());
            emit dataChanged(modelIndex1, modelIndex1);
            emit dataChanged(modelIndex5, modelIndex5);
            invalidateDependencies(binding, modelIndex1);
            return;
        }
    }
}

void GammaRay::QmlBindingModel::invalidateDependencies(GammaRay::QmlBindingNode* node, const QModelIndex &nodeIndex)
{
    emit dataChanged(index(0, 1, nodeIndex), index(node->dependencies().size() - 1, 1, nodeIndex));
    emit dataChanged(index(0, 5, nodeIndex), index(node->dependencies().size() - 1, 5, nodeIndex));
    for (int i = 0; i < node->dependencies().size(); i++) {
        invalidateDependencies(node->dependencies()[i].get(), index(i, 1, nodeIndex));
    }
}

std::vector<QmlBindingNode *> QmlBindingModel::bindingsFromObject(QObject* obj)
{
    std::vector<QmlBindingNode *> bindings;
    if (!obj)
        return bindings;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
    if (!data)
        return bindings;

//     for (int i = data->propertyCache->propertyOffset(); i < data->propertyCache->propertyCount(); ++i) {
//         QQmlPropertyData *prop = data->propertyCache->property(i);
//     }

    auto b = data->bindings;
    while (b) {
        if (auto qmlBinding = dynamic_cast<QQmlBinding*>(b)) {
            QmlBindingNode *node;

            // Try to find out dependencies of this binding.
            node = new QmlBindingNode(qmlBinding);

            QMetaObject::connect(obj, node->property().notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"));

            bindings.push_back(node);
        } else {
            qDebug() << "Ohhh...";
        }
//         if (node->expression().isEmpty())
//             node->expression() = b->expression();
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
                int depth = binding->depth();
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
        index = createIndex(row, column, m_bindings[row]);
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
        for (int i = 0; i < m_bindings.size(); i++) {
            if (parent == m_bindings[i]) {
                return createIndex(i, child.column(), m_bindings[i]);
            }
        }
    }
    for (int i = 0; i < grandparent->dependencies().size(); i++) {
        if (parent == grandparent->dependencies()[i].get()) {
            return createIndex(i, child.column(), grandparent->dependencies()[i].get());
        }
    }
    return QModelIndex();
}
