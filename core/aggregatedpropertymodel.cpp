/*
  aggregatedpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "aggregatedpropertymodel.h"
#include "metaobjectrepository.h"
#include "objectinstance.h"
#include "probe.h"
#include "propertyaggregator.h"
#include "propertydata.h"
#include "propertyadaptorfactory.h"
#include "toolfactory.h"
#include "varianthandler.h"
#include "enumrepositoryserver.h"
#include "enumutil.h"
#include "util.h"

#include <common/objectid.h>
#include <common/propertymodel.h>

#include <QDebug>
#include <QMetaEnum>

using namespace GammaRay;


Q_DECLARE_METATYPE(const QObject*)

/*!
 * Checks if it is dangerous to unpack a QVariant because it stores an invalid pointer.
 * Returns true if the value is an invalid pointer to a QObject.
 * Returns false either if it is a valid pointer or not a QObject*.
 */
bool isInvalidPointer(const QVariant& value) {
    if (!value.canConvert<QObject *>() && !value.canConvert<const QObject *>())
        return false;
    return !Probe::instance()->isValidObject(Util::uncheckedQObjectCast(value));
}


AggregatedPropertyModel::AggregatedPropertyModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    qRegisterMetaType<GammaRay::PropertyAdaptor *>();
}

AggregatedPropertyModel::~AggregatedPropertyModel() = default;

void AggregatedPropertyModel::setObject(const ObjectInstance &oi)
{
    clear();

    if (!oi.isValid())
        return;
    auto adaptor = PropertyAdaptorFactory::create(oi, this);
    if (!adaptor)
        return;

    auto count = adaptor->count();
    if (count)
        beginInsertRows(QModelIndex(), 0, count -1);

    m_rootAdaptor = adaptor;
    addPropertyAdaptor(m_rootAdaptor);

    if (count)
        endInsertRows();
}

void AggregatedPropertyModel::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}

void AggregatedPropertyModel::clear()
{
    if (!m_rootAdaptor)
        return;

    const auto count = m_parentChildrenMap.value(m_rootAdaptor).size();
    if (count)
        beginRemoveRows(QModelIndex(), 0, count - 1);

    m_parentChildrenMap.clear();
    delete m_rootAdaptor;
    m_rootAdaptor = nullptr;

    if (count)
        endRemoveRows();
}

QVariant AggregatedPropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_rootAdaptor)
        return QVariant();

    const auto adaptor = adaptorForIndex(index);
    if (!adaptor->object().isValid()) {
        QMetaObject::invokeMethod(const_cast<AggregatedPropertyModel *>(this), "objectInvalidated",
                                  Qt::QueuedConnection,
                                  Q_ARG(GammaRay::PropertyAdaptor*, adaptor));
        return QVariant();
    }

    const auto d = adaptor->propertyData(index.row());
    return data(adaptor, d, index.column(), role);
}

QMap<int, QVariant> AggregatedPropertyModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> res;
    if (!index.isValid() || !m_rootAdaptor)
        return res;

    const auto adaptor = adaptorForIndex(index);
    if (!adaptor->object().isValid()) {
        QMetaObject::invokeMethod(const_cast<AggregatedPropertyModel *>(this), "objectInvalidated",
                                  Qt::QueuedConnection,
                                  Q_ARG(GammaRay::PropertyAdaptor*, adaptor));
        return res;
    }
    const auto d = adaptor->propertyData(index.row());

    res.insert(Qt::DisplayRole, data(adaptor, d, index.column(), Qt::DisplayRole));
    res.insert(PropertyModel::ActionRole, data(adaptor, d,
                                               index.column(), PropertyModel::ActionRole));
    res.insert(PropertyModel::ObjectIdRole,
               data(adaptor, d, index.column(), PropertyModel::ObjectIdRole));
    if (index.column() == 0) {
        auto v = data(adaptor, d, index.column(), PropertyModel::PropertyFlagsRole);
        if (!v.isNull())
            res.insert(PropertyModel::PropertyFlagsRole, v);
        v = data(adaptor, d, index.column(), PropertyModel::PropertyRevisionRole);
        if (!v.isNull())
            res.insert(PropertyModel::PropertyRevisionRole, v);
        v = data(adaptor, d, index.column(), PropertyModel::NotifySignalRole);
        if (!v.isNull())
            res.insert(PropertyModel::NotifySignalRole, v);
    } else if (index.column() == 1) {
        res.insert(Qt::EditRole, data(adaptor, d, index.column(), Qt::EditRole));
        res.insert(Qt::DecorationRole, data(adaptor, d, index.column(), Qt::DecorationRole));
        if (d.value().type() == QVariant::Bool)
            res.insert(Qt::CheckStateRole, data(adaptor, d, index.column(), Qt::CheckStateRole));
    }
    return res;
}

QVariant AggregatedPropertyModel::data(PropertyAdaptor *adaptor, const PropertyData &d, int column,
                                       int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        switch (column) {
        case 0:
            return d.name();
        case 1:
        {
            // QMetaProperty::read sets QVariant::typeName to int for enums,
            // so we need to handle that separately here
            const QString enumStr = EnumUtil::enumToString(d.value(), d.typeName().toLatin1(), adaptor->object().metaObject());
            if (!enumStr.isEmpty())
                return enumStr;
            if (d.value().type() == QVariant::Bool && (d.accessFlags() & PropertyData::Writable))
                return QVariant();
            if (isInvalidPointer(d.value()) && Util::uncheckedQObjectCast(d.value()) != nullptr)
                return "[invalid]";
            return VariantHandler::displayString(d.value());
        }
        case 2:
            return d.typeName();
        case 3:
            return d.className();
        }
        break;
    case Qt::EditRole:
        if (column == 1) {
            const auto me = EnumUtil::metaEnum(d.value(), d.typeName().toLatin1(), adaptor->object().metaObject());
            if (me.isValid()) {
                const auto num = EnumUtil::enumToInt(d.value(), me);
                return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
            }
            return VariantHandler::serializableVariant(d.value());
        }
        break;
    case Qt::DecorationRole:
        if (column == 1)
            return VariantHandler::decoration(d.value());
        break;
    case Qt::CheckStateRole:
        if (column == 1 && d.value().type() == QVariant::Bool && (d.accessFlags() & PropertyData::Writable))
            return d.value().toBool() ? Qt::Checked : Qt::Unchecked;
        break;
    case PropertyModel::ActionRole:
    {
        int actions = PropertyModel::NoAction;
        if (d.accessFlags() & PropertyData::Resettable)
            actions |= PropertyModel::Reset;
        if (d.accessFlags() & PropertyData::Deletable)
            actions |= PropertyModel::Delete;
        if ((MetaObjectRepository::instance()->metaObject(d.typeName())
             && *reinterpret_cast<void * const *>(d.value().data()))
            || d.value().value<QObject *>())
            actions |= PropertyModel::NavigateTo;
        return actions;
    }
    case PropertyModel::ObjectIdRole:
        if (d.value().canConvert<QObject *>()) {
            if (isInvalidPointer(d.value()))
                return QVariant();
            return QVariant::fromValue(ObjectId(d.value().value<QObject *>()));
        } else if (d.value().isValid()) {
            const auto &v = d.value();
            return QVariant::fromValue(ObjectId(*reinterpret_cast<void * const *>(v.data()),
                                                v.typeName()));
        }
        return QVariant();
    case PropertyModel::PropertyFlagsRole:
        if (column == 0 && d.propertyFlags() != PropertyModel::None)
            return QVariant::fromValue(d.propertyFlags());
        return QVariant();
    case PropertyModel::PropertyRevisionRole:
        if (column == 0 && d.revision() >= 0)
            return d.revision();
        return QVariant();
    case PropertyModel::NotifySignalRole:
        if (column == 0 && !d.notifySignal().isEmpty())
            return d.notifySignal();
        return QVariant();
    }
    return QVariant();
}

bool AggregatedPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_rootAdaptor)
        return false;

    const auto adaptor = adaptorForIndex(index);
    switch (role) {
    case Qt::EditRole:
    {
        if (value.userType() == qMetaTypeId<EnumValue>()) {
            const auto d = adaptor->propertyData(index.row());
            if (d.value().type() == QVariant::Int) {
                adaptor->writeProperty(index.row(), value.value<EnumValue>().value());
            } else {
                auto v = d.value();
                *(static_cast<int*>(v.data())) = value.value<EnumValue>().value();
                adaptor->writeProperty(index.row(), v);
            }
        } else {
            adaptor->writeProperty(index.row(), value);
        }
        propagateWrite(adaptor);
        return true;
    }
    case Qt::CheckStateRole:
        adaptor->writeProperty(index.row(), value.toInt() == Qt::Checked);
        propagateWrite(adaptor);
        return true;
    case PropertyModel::ResetActionRole:
        adaptor->resetProperty(index.row());
        return true;
    }

    return false;
}

int AggregatedPropertyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int AggregatedPropertyModel::rowCount(const QModelIndex &parent) const
{
    if (!m_rootAdaptor || parent.column() > 0)
        return 0;
    if (!parent.isValid())
        return m_parentChildrenMap.value(m_rootAdaptor).size();

    auto adaptor = adaptorForIndex(parent);
    auto &siblings = m_parentChildrenMap[adaptor];
    if (!m_inhibitAdaptorCreation && !siblings.at(parent.row())) {
        // TODO: remember we tried any of this
        auto pd = adaptor->propertyData(parent.row());
        if (!isInvalidPointer(pd.value()) && !hasLoop(adaptor, pd.value())) {
            auto a = PropertyAdaptorFactory::create(pd.value(), adaptor);
            siblings[parent.row()] = a;
            addPropertyAdaptor(a);
        }
    }
    auto childAdaptor = siblings.at(parent.row());
    if (!childAdaptor)
        return 0;
    return m_parentChildrenMap.value(childAdaptor).size(); // childAdaptor->count() might already be updated in insert/removeRows
}

Qt::ItemFlags AggregatedPropertyModel::flags(const QModelIndex &index) const
{
    const auto baseFlags = QAbstractItemModel::flags(index);
    if (!index.isValid() || index.column() != 1 || m_readOnly)
        return baseFlags;

    auto adaptor = adaptorForIndex(index);
    auto data = adaptor->propertyData(index.row());
    const auto editable = (data.accessFlags() & PropertyData::Writable) && isParentEditable(adaptor);
    const auto booleanEditable = editable && data.value().type() == QVariant::Bool;
    if (booleanEditable)
        return baseFlags | Qt::ItemIsUserCheckable;
    return editable ? (baseFlags | Qt::ItemIsEditable) : baseFlags;
}

QModelIndex AggregatedPropertyModel::parent(const QModelIndex &child) const
{
    auto childAdaptor = adaptorForIndex(child);
    if (childAdaptor == m_rootAdaptor)
        return {};

    auto parentAdaptor = childAdaptor->parentAdaptor();
    return createIndex(m_parentChildrenMap.value(parentAdaptor).indexOf(
                           childAdaptor), 0, parentAdaptor);
}

QModelIndex AggregatedPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent) || !m_rootAdaptor)
        return {};

    if (!parent.isValid())
        return createIndex(row, column, m_rootAdaptor);
    auto adaptor = adaptorForIndex(parent);
    return createIndex(row, column, m_parentChildrenMap.value(adaptor).at(parent.row()));
}

PropertyAdaptor *AggregatedPropertyModel::adaptorForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return m_rootAdaptor;
    return static_cast<PropertyAdaptor *>(index.internalPointer());
}

void AggregatedPropertyModel::addPropertyAdaptor(PropertyAdaptor *adaptor) const
{
    if (!adaptor)
        return;
    QVector<PropertyAdaptor *> children;
    children.resize(adaptor->count());
    m_parentChildrenMap.insert(adaptor, children);
    connect(adaptor, &PropertyAdaptor::propertyChanged, this, &AggregatedPropertyModel::propertyChanged);
    connect(adaptor, &PropertyAdaptor::propertyAdded, this, &AggregatedPropertyModel::propertyAdded);
    connect(adaptor, &PropertyAdaptor::propertyRemoved, this, &AggregatedPropertyModel::propertyRemoved);
}

void AggregatedPropertyModel::propertyChanged(int first, int last)
{
    auto adaptor = qobject_cast<PropertyAdaptor *>(sender());
    Q_ASSERT(adaptor);
    Q_ASSERT(m_parentChildrenMap.contains(adaptor));
    Q_ASSERT(first <= last);
    Q_ASSERT(first >= 0);
    Q_ASSERT(last < adaptor->count());

    emit dataChanged(createIndex(first, 0, adaptor), createIndex(last, columnCount() - 1, adaptor));
    for (int i = first; i <= last; ++i)
        reloadSubTree(adaptor, i);
}

void AggregatedPropertyModel::propertyAdded(int first, int last)
{
    auto adaptor = qobject_cast<PropertyAdaptor *>(sender());
    Q_ASSERT(adaptor);
    Q_ASSERT(m_parentChildrenMap.contains(adaptor));
    Q_ASSERT(first <= last);
    Q_ASSERT(first >= 0);
    Q_ASSERT(last < adaptor->count());

    auto idx = createIndex(first, 0, adaptor);
    beginInsertRows(idx.parent(), first, last);
    auto &children = m_parentChildrenMap[adaptor];
    if (first >= children.size())
        children.resize(last + 1);
    else
        children.insert(first, last - first + 1, nullptr);
    endInsertRows();
}

void AggregatedPropertyModel::propertyRemoved(int first, int last)
{
    auto adaptor = qobject_cast<PropertyAdaptor *>(sender());
    Q_ASSERT(adaptor);
    Q_ASSERT(m_parentChildrenMap.contains(adaptor));
    Q_ASSERT(first <= last);
    Q_ASSERT(first >= 0);
    Q_ASSERT(last < adaptor->count());

    auto idx = createIndex(first, 0, adaptor);
    beginRemoveRows(idx.parent(), first, last);
    auto &children = m_parentChildrenMap[adaptor];
    children.remove(first, last - first + 1);
    endRemoveRows();
}

void AggregatedPropertyModel::objectInvalidated()
{
    auto adaptor = qobject_cast<PropertyAdaptor *>(sender());
    objectInvalidated(adaptor);
}

void AggregatedPropertyModel::objectInvalidated(PropertyAdaptor *adaptor)
{
    Q_ASSERT(adaptor);
    if (!m_parentChildrenMap.contains(adaptor)) // already handled
        return;

    if (adaptor == m_rootAdaptor) {
        clear();
        return;
    }

    auto parentAdaptor = adaptor->parentAdaptor();
    Q_ASSERT(parentAdaptor);
    Q_ASSERT(m_parentChildrenMap.contains(parentAdaptor));
    reloadSubTree(parentAdaptor, m_parentChildrenMap.value(parentAdaptor).indexOf(adaptor));
}

bool AggregatedPropertyModel::hasLoop(PropertyAdaptor *adaptor, const QVariant &v) const
{
    const ObjectInstance newOi(v);
    if (newOi.type() != ObjectInstance::QtObject && newOi.type() != ObjectInstance::Object)
        return false;
    if (!newOi.object())
        return false;

    while (adaptor) {
        if (adaptor->object() == newOi)
            return true;
        adaptor = adaptor->parentAdaptor();
    }

    return false;
}

void AggregatedPropertyModel::reloadSubTree(PropertyAdaptor *parentAdaptor, int index)
{
    Q_ASSERT(parentAdaptor);
    Q_ASSERT(m_parentChildrenMap.contains(parentAdaptor));
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_parentChildrenMap.value(parentAdaptor).size());

    // prevent rowCount calls as a result of the change notification to re-create
    // the adaptor
    m_inhibitAdaptorCreation = true;

    // remove the old sub-tree, if present
    auto oldAdaptor = m_parentChildrenMap.value(parentAdaptor).at(index);
    if (oldAdaptor) {
        auto oldRowCount = m_parentChildrenMap.value(oldAdaptor).size();
        if (oldRowCount > 0)
            beginRemoveRows(createIndex(index, 0, parentAdaptor), 0, oldRowCount - 1);
        m_parentChildrenMap[parentAdaptor][index] = nullptr;
        m_parentChildrenMap.remove(oldAdaptor);
        delete oldAdaptor;
        if (oldRowCount)
            endRemoveRows();
    }

    // re-add the sub-tree
    // TODO consolidate with code in rowCount()
    auto pd = parentAdaptor->propertyData(index);

    if (isInvalidPointer(pd.value()) || hasLoop(parentAdaptor, pd.value())) {
        m_inhibitAdaptorCreation = false;
        return;
    }
    auto newAdaptor = PropertyAdaptorFactory::create(pd.value(), parentAdaptor);
    if (!newAdaptor) {
        m_inhibitAdaptorCreation = false;
        return;
    }
    auto newRowCount = newAdaptor->count();
    if (newRowCount > 0)
        beginInsertRows(createIndex(index, 0, parentAdaptor), 0, newRowCount - 1);
    m_parentChildrenMap[parentAdaptor][index] = newAdaptor;
    addPropertyAdaptor(newAdaptor);
    if (newRowCount > 0)
        endInsertRows();

    m_inhibitAdaptorCreation = false;
}

bool AggregatedPropertyModel::isParentEditable(PropertyAdaptor *adaptor) const
{
    const auto parentAdaptor = adaptor->parentAdaptor();
    if (!parentAdaptor)
        return true;

    // we need all value types along the way to be writable
    if (adaptor->object().isValueType()) {
        const auto row = m_parentChildrenMap.value(parentAdaptor).indexOf(adaptor);
        Q_ASSERT(row >= 0);

        const auto pd = parentAdaptor->propertyData(row);
        if ((pd.accessFlags() & PropertyData::Writable) == 0)
            return false;
    }

    return isParentEditable(parentAdaptor);
}

void AggregatedPropertyModel::propagateWrite(GammaRay::PropertyAdaptor* adaptor)
{
    const auto parentAdaptor = adaptor->parentAdaptor();
    if (!parentAdaptor)
        return;

    if (adaptor->object().isValueType()) {
        const auto row = m_parentChildrenMap.value(parentAdaptor).indexOf(adaptor);
        Q_ASSERT(row >= 0);

        parentAdaptor->writeProperty(row, adaptor->object().variant());
    }

    propagateWrite(parentAdaptor);
}
