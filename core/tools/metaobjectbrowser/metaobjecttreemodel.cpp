/*
  metaobjecttreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "metaobjecttreemodel.h"

#include <core/probe.h>
#include <core/qmetaobjectvalidator.h>
#include <core/metaobjectregistry.h>

#include <common/metatypedeclarations.h>
#include <common/tools/metaobjectbrowser/qmetaobjectmodel.h>

#include <compat/qasconst.h>

#include <QDebug>
#include <QThread>
#include <QTimer>

#include <cassert>

using namespace GammaRay;

static inline MetaObjectRegistry *registry()
{
    return Probe::instance()->metaObjectRegistry();
}

MetaObjectTreeModel::MetaObjectTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_pendingDataChangedTimer(new QTimer(this))
{
    connect(registry(), &MetaObjectRegistry::beforeMetaObjectAdded, this, &MetaObjectTreeModel::addMetaObject);
    connect(registry(), &MetaObjectRegistry::afterMetaObjectAdded, this, &MetaObjectTreeModel::endAddMetaObject);
    connect(registry(), &MetaObjectRegistry::dataChanged, this, &MetaObjectTreeModel::scheduleDataChange);

    m_pendingDataChangedTimer->setInterval(100);
    m_pendingDataChangedTimer->setSingleShot(true);
    connect(m_pendingDataChangedTimer, &QTimer::timeout, this, &MetaObjectTreeModel::emitPendingDataChanged);
}

MetaObjectTreeModel::~MetaObjectTreeModel() = default;

QVariant MetaObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int column = index.column();
    const QMetaObject *object = metaObjectForIndex(index);

    if (role == Qt::DisplayRole) {
        switch (column) {
        case QMetaObjectModel::ObjectColumn:
            return registry()->data(object, MetaObjectRegistry::ClassName);
        case QMetaObjectModel::ObjectSelfCountColumn:
            return registry()->data(object, MetaObjectRegistry::SelfCount);
        case QMetaObjectModel::ObjectInclusiveCountColumn:
            return registry()->data(object, MetaObjectRegistry::InclusiveCount);
        case QMetaObjectModel::ObjectSelfAliveCountColumn:
            return registry()->data(object, MetaObjectRegistry::SelfAliveCount);
        case QMetaObjectModel::ObjectInclusiveAliveCountColumn:
            return registry()->data(object, MetaObjectRegistry::InclusiveAliveCount);
        default:
            break;
        }
    } else if (role == QMetaObjectModel::MetaObjectRole) {
        if (!registry()->isValid(object))
            return QVariant();
        return QVariant::fromValue<const QMetaObject *>(registry()->aliveInstance(object));
    } else if (role == QMetaObjectModel::MetaObjectIssues && index.column() == QMetaObjectModel::ObjectColumn) {
        if (!registry()->isStatic(object))
            return QVariant();
        const auto r = QMetaObjectValidator::check(registry()->aliveInstance(object));
        return r == QMetaObjectValidatorResult::NoIssue ? QVariant() : QVariant::fromValue(r);
    } else if (role == QMetaObjectModel::MetaObjectInvalid && index.column() == QMetaObjectModel::ObjectInclusiveAliveCountColumn) {
        if (!registry()->isValid(object))
            return true;
        return QVariant();
    }
    return QVariant();
}

int MetaObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return QMetaObjectModel::_Last;
}

int MetaObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    const QMetaObject *metaObject = metaObjectForIndex(parent);
    return registry()->childrenOf(metaObject).size();
}

QModelIndex MetaObjectTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return {};

    const QMetaObject *object = metaObjectForIndex(child);
    Q_ASSERT(object);
    const QMetaObject *parentObject = registry()->parentOf(object);
    return indexForMetaObject(parentObject);
}

QModelIndex MetaObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    const QMetaObject *parentObject = metaObjectForIndex(parent);
    const QVector<const QMetaObject *> &children = registry()->childrenOf(parentObject);
    if (row < 0 || column < 0 || row >= children.size() || column >= columnCount())
        return {};

    const QMetaObject *object = children.at(row);
    return createIndex(row, column, const_cast<QMetaObject *>(object));
}

QModelIndexList MetaObjectTreeModel::match(const QModelIndex &start, int role,
                                           const QVariant &value, int hits,
                                           Qt::MatchFlags flags) const
{
    if (role == QMetaObjectModel::MetaObjectRole) {
        const auto mo = value.value<const QMetaObject *>();
        return QModelIndexList() << indexForMetaObject(mo);
    }
    return QAbstractItemModel::match(start, role, value, hits, flags);
}

void MetaObjectTreeModel::addMetaObject(const QMetaObject *metaObject)
{
    const QMetaObject *parentMetaObject = registry()->parentOf(metaObject);

    const QModelIndex parentIndex = indexForMetaObject(parentMetaObject);
    // either we get a proper parent and hence valid index or there is no parent
    assert(parentIndex.isValid() || !parentMetaObject);

    int size = registry()->childrenOf(parentMetaObject).size();
    beginInsertRows(parentIndex, size, size);
}

void MetaObjectTreeModel::endAddMetaObject(const QMetaObject *metaObject)
{
    Q_UNUSED(metaObject)

    endInsertRows();
}

QModelIndex MetaObjectTreeModel::indexForMetaObject(const QMetaObject *metaObject) const
{
    if (!metaObject)
        return {};

    const QMetaObject *parentObject = registry()->parentOf(metaObject);
    Q_ASSERT(parentObject != metaObject);
    const QModelIndex parentIndex = indexForMetaObject(parentObject);
    if (!parentIndex.isValid() && parentObject)
        return QModelIndex();

    const int row = registry()->childrenOf(parentObject).indexOf(metaObject);
    if (row < 0)
        return QModelIndex();

    return index(row, 0, parentIndex);
}

const QMetaObject *MetaObjectTreeModel::metaObjectForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    void *internalPointer = index.internalPointer();
    const QMetaObject *metaObject = reinterpret_cast<QMetaObject *>(internalPointer);
    return metaObject;
}

void GammaRay::MetaObjectTreeModel::scheduleDataChange(const QMetaObject *mo)
{
    m_pendingDataChanged.insert(mo);
    if (!m_pendingDataChangedTimer->isActive())
        m_pendingDataChangedTimer->start();
}

void GammaRay::MetaObjectTreeModel::emitPendingDataChanged()
{
    for (auto mo : qAsConst(m_pendingDataChanged)) {
        auto index = indexForMetaObject(mo);
        if (!index.isValid())
            continue;
        emit dataChanged(index.sibling(index.row(), QMetaObjectModel::ObjectSelfCountColumn),
                         index.sibling(index.row(), QMetaObjectModel::ObjectInclusiveAliveCountColumn));
    }
    m_pendingDataChanged.clear();
}
