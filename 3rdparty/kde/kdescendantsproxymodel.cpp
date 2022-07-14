/*
    SPDX-FileCopyrightText: 2009 Stephen Kelly <steveire@gmail.com>
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kdescendantsproxymodel.h"
#include "kbihash_p.h"

#include <QStringList>


typedef KHash2Map<QPersistentModelIndex, int> Mapping;

class KDescendantsProxyModelPrivate
{
    KDescendantsProxyModelPrivate(KDescendantsProxyModel *qq)
        : q_ptr(qq)
        , m_rowCount(0)
        , m_ignoreNextLayoutAboutToBeChanged(false)
        , m_ignoreNextLayoutChanged(false)
        , m_relayouting(false)
        , m_displayAncestorData(false)
        , m_ancestorSeparator(QStringLiteral(" / "))
    {
    }

    Q_DECLARE_PUBLIC(KDescendantsProxyModel)
    KDescendantsProxyModel *const q_ptr;

    mutable QVector<QPersistentModelIndex> m_pendingParents;

    void scheduleProcessPendingParents() const;
    void processPendingParents();

    void synchronousMappingRefresh();

    void updateInternalIndexes(int start, int offset);

    void resetInternalData();

    void notifyhasSiblings(const QModelIndex &parent);
    void sourceRowsAboutToBeInserted(const QModelIndex &, int, int);
    void sourceRowsInserted(const QModelIndex &, int, int);
    void sourceRowsAboutToBeRemoved(const QModelIndex &, int, int);
    void sourceRowsRemoved(const QModelIndex &, int, int);
    void sourceRowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int);
    void sourceRowsMoved(const QModelIndex &, int, int, const QModelIndex &, int);
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sourceLayoutAboutToBeChanged();
    void sourceLayoutChanged();
    void sourceDataChanged(const QModelIndex &, const QModelIndex &);
    void sourceModelDestroyed();

    Mapping m_mapping;
    int m_rowCount;
    QPair<int, int> m_removePair;
    QPair<int, int> m_insertPair;

    bool m_expandsByDefault = true;
    bool m_ignoreNextLayoutAboutToBeChanged;
    bool m_ignoreNextLayoutChanged;
    bool m_relayouting;

    bool m_displayAncestorData;
    QString m_ancestorSeparator;

    QSet<QPersistentModelIndex> m_expandedSourceIndexes;
    QSet<QPersistentModelIndex> m_collapsedSourceIndexes;

    QList<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_proxyIndexes;
};

void KDescendantsProxyModelPrivate::resetInternalData()
{
    m_rowCount = 0;
    m_mapping.clear();
    m_layoutChangePersistentIndexes.clear();
    m_proxyIndexes.clear();
}

void KDescendantsProxyModelPrivate::synchronousMappingRefresh()
{
    m_rowCount = 0;
    m_mapping.clear();
    m_pendingParents.clear();

    m_pendingParents.append(QModelIndex());

    m_relayouting = true;
    while (!m_pendingParents.isEmpty()) {
        processPendingParents();
    }
    m_relayouting = false;
}

void KDescendantsProxyModelPrivate::scheduleProcessPendingParents() const
{
    const_cast<KDescendantsProxyModelPrivate *>(this)->processPendingParents();
}

void KDescendantsProxyModelPrivate::processPendingParents()
{
    Q_Q(KDescendantsProxyModel);
    const QVector<QPersistentModelIndex>::iterator begin = m_pendingParents.begin();
    QVector<QPersistentModelIndex>::iterator it = begin;

    const QVector<QPersistentModelIndex>::iterator end = m_pendingParents.end();

    QVector<QPersistentModelIndex> newPendingParents;

    while (it != end && it != m_pendingParents.end()) {
        const QModelIndex sourceParent = *it;
        if (!sourceParent.isValid() && m_rowCount > 0) {
            // It was removed from the source model before it was inserted.
            it = m_pendingParents.erase(it);
            continue;
        }
        if (!q->isSourceIndexVisible(sourceParent)) {
            // It's a collapsed node, or its parents are collapsed, ignore.
            it = m_pendingParents.erase(it);
            continue;
        }

        const int rowCount = q->sourceModel()->rowCount(sourceParent);

        // A node can be marked as collapsed or expanded even if it doesn't have children
        if (rowCount == 0) {
            it = m_pendingParents.erase(it);
            continue;
        }
        const QPersistentModelIndex sourceIndex = q->sourceModel()->index(rowCount - 1, 0, sourceParent);

        Q_ASSERT(sourceIndex.isValid());

        const QModelIndex proxyParent = q->mapFromSource(sourceParent);

        Q_ASSERT(sourceParent.isValid() == proxyParent.isValid());
        const int proxyEndRow = proxyParent.row() + rowCount;
        const int proxyStartRow = proxyEndRow - rowCount + 1;

        if (!m_relayouting) {
            q->beginInsertRows(QModelIndex(), proxyStartRow, proxyEndRow);
        }

        updateInternalIndexes(proxyStartRow, rowCount);
        m_mapping.insert(sourceIndex, proxyEndRow);
        it = m_pendingParents.erase(it);
        m_rowCount += rowCount;

        if (!m_relayouting) {
            q->endInsertRows();
        }

        for (int sourceRow = 0; sourceRow < rowCount; ++sourceRow) {
            static const int column = 0;
            const QModelIndex child = q->sourceModel()->index(sourceRow, column, sourceParent);
            Q_ASSERT(child.isValid());

            if (q->sourceModel()->hasChildren(child) && q->isSourceIndexExpanded(child) && q->sourceModel()->rowCount(child) > 0) {
                newPendingParents.append(child);
            }
        }
    }
    m_pendingParents += newPendingParents;
    if (!m_pendingParents.isEmpty()) {
        processPendingParents();
    }
    //   scheduleProcessPendingParents();
}

void KDescendantsProxyModelPrivate::updateInternalIndexes(int start, int offset)
{
    // TODO: Make KHash2Map support key updates and do this backwards.
    QHash<int, QPersistentModelIndex> updates;
    {
        Mapping::right_iterator it = m_mapping.rightLowerBound(start);
        const Mapping::right_iterator end = m_mapping.rightEnd();

        while (it != end) {
            updates.insert(it.key() + offset, *it);
            ++it;
        }
    }

    {
        QHash<int, QPersistentModelIndex>::const_iterator it = updates.constBegin();
        const QHash<int, QPersistentModelIndex>::const_iterator end = updates.constEnd();

        for (; it != end; ++it) {
            m_mapping.insert(it.value(), it.key());
        }
    }
}

KDescendantsProxyModel::KDescendantsProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
    , d_ptr(new KDescendantsProxyModelPrivate(this))
{
}

KDescendantsProxyModel::~KDescendantsProxyModel()
{
    delete d_ptr;
}

QHash<int, QByteArray> KDescendantsProxyModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractProxyModel::roleNames();

    roleNames[LevelRole] = "kDescendantLevel";
    roleNames[ExpandableRole] = "kDescendantExpandable";
    roleNames[ExpandedRole] = "kDescendantExpanded";
    roleNames[HasSiblingsRole] = "kDescendantHasSiblings";
    return roleNames;
}

void KDescendantsProxyModel::setExpandsByDefault(bool expand)
{
    if (d_ptr->m_expandsByDefault == expand) {
        return;
    }

    beginResetModel();
    d_ptr->m_expandsByDefault = expand;
    d_ptr->m_expandedSourceIndexes.clear();
    d_ptr->m_collapsedSourceIndexes.clear();
    endResetModel();
}

bool KDescendantsProxyModel::expandsByDefault() const
{
    return d_ptr->m_expandsByDefault;
}

bool KDescendantsProxyModel::isSourceIndexExpanded(const QModelIndex &sourceIndex) const
{
    // Root is always expanded
    if (!sourceIndex.isValid()) {
        return true;
    } else if (d_ptr->m_expandsByDefault) {
        return !d_ptr->m_collapsedSourceIndexes.contains(QPersistentModelIndex(sourceIndex));
    } else {
        return d_ptr->m_expandedSourceIndexes.contains(QPersistentModelIndex(sourceIndex));
    }
}

bool KDescendantsProxyModel::isSourceIndexVisible(const QModelIndex &sourceIndex) const
{
    // Root is always visible
    if (!sourceIndex.isValid()) {
        return true;
    }

    QModelIndex index(sourceIndex);
    do {
        index = index.parent();
        if (!index.isValid()) {
            return true;
        }
    } while (isSourceIndexExpanded(index));

    return false;
}

void KDescendantsProxyModel::expandSourceIndex(const QModelIndex &sourceIndex)
{
    if (!sourceIndex.isValid() || isSourceIndexExpanded(sourceIndex)) {
        return;
    }

    if (d_ptr->m_expandsByDefault) {
        d_ptr->m_collapsedSourceIndexes.remove(QPersistentModelIndex(sourceIndex));
    } else {
        d_ptr->m_expandedSourceIndexes << QPersistentModelIndex(sourceIndex);
    }

    d_ptr->m_pendingParents << sourceIndex;
    d_ptr->scheduleProcessPendingParents();
    Q_EMIT sourceIndexExpanded(sourceIndex);

    const QModelIndex index = mapFromSource(sourceIndex);
    Q_EMIT dataChanged(index, index, {ExpandedRole});
}

void KDescendantsProxyModel::collapseSourceIndex(const QModelIndex &sourceIndex)
{
    if (!sourceIndex.isValid() || !isSourceIndexExpanded(sourceIndex)) {
        return;
    }

    const int row = mapFromSource(sourceIndex).row();
    const int rowStart = row + 1;
    int rowEnd = row;

    QList<QModelIndex> toVisit = {sourceIndex};
    QSet<QModelIndex> visited;
    while (!toVisit.isEmpty()) {
        QModelIndex index = toVisit.takeLast();
        if (!visited.contains(index)) {
            visited << index;
            const int nRows = sourceModel()->rowCount(index);
            rowEnd += nRows;
            for (int i = 0; i < nRows; ++i) {
                QModelIndex child = sourceModel()->index(i, 0, index);
                if (isSourceIndexExpanded(child)) {
                    toVisit << child;
                }
            }
        }
    }

    if (d_ptr->m_expandsByDefault) {
        d_ptr->m_collapsedSourceIndexes << QPersistentModelIndex(sourceIndex);
    } else {
        d_ptr->m_expandedSourceIndexes.remove(QPersistentModelIndex(sourceIndex));
    }

    {
        Mapping::right_iterator it = d_ptr->m_mapping.rightLowerBound(rowStart);
        const Mapping::right_iterator endIt = d_ptr->m_mapping.rightUpperBound(rowEnd);

        if (endIt != d_ptr->m_mapping.rightEnd()) {
            while (it != endIt) {
                it = d_ptr->m_mapping.eraseRight(it);
            }
        } else {
            while (it != d_ptr->m_mapping.rightUpperBound(rowEnd)) {
                it = d_ptr->m_mapping.eraseRight(it);
            }
        }
    }

    d_ptr->m_removePair = qMakePair(rowStart, rowEnd);

    beginRemoveRows(QModelIndex(), rowStart, rowEnd);
    d_ptr->synchronousMappingRefresh();
    endRemoveRows();
    Q_EMIT sourceIndexCollapsed(sourceIndex);

    const QModelIndex ownIndex = mapFromSource(sourceIndex);
    Q_EMIT dataChanged(ownIndex, ownIndex, {ExpandedRole});
}

QModelIndexList KDescendantsProxyModel::match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const
{
    return QAbstractProxyModel::match(start, role, value, hits, flags);
}

namespace
{
// we only work on DisplayRole for now
static const QVector<int> changedRoles = {Qt::DisplayRole};
}

void KDescendantsProxyModel::setDisplayAncestorData(bool display)
{
    Q_D(KDescendantsProxyModel);
    bool displayChanged = (display != d->m_displayAncestorData);
    d->m_displayAncestorData = display;
    if (displayChanged) {
        Q_EMIT displayAncestorDataChanged();
        // send out big hammer. Everything needs to be updated.
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), changedRoles);
    }
}

bool KDescendantsProxyModel::displayAncestorData() const
{
    Q_D(const KDescendantsProxyModel);
    return d->m_displayAncestorData;
}

void KDescendantsProxyModel::setAncestorSeparator(const QString &separator)
{
    Q_D(KDescendantsProxyModel);
    bool separatorChanged = (separator != d->m_ancestorSeparator);
    d->m_ancestorSeparator = separator;
    if (separatorChanged) {
        Q_EMIT ancestorSeparatorChanged();
        if (d->m_displayAncestorData) {
            // send out big hammer. Everything needs to be updated.
            Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), changedRoles);
        }
    }
}

QString KDescendantsProxyModel::ancestorSeparator() const
{
    Q_D(const KDescendantsProxyModel);
    return d->m_ancestorSeparator;
}

void KDescendantsProxyModel::setSourceModel(QAbstractItemModel *_sourceModel)
{
    Q_D(KDescendantsProxyModel);

    beginResetModel();

    if (sourceModel()) {
        disconnect(sourceModel(), nullptr, this, nullptr);
    }

    QAbstractProxyModel::setSourceModel(_sourceModel);
    d_ptr->m_expandedSourceIndexes.clear();

    if (_sourceModel) {
        connect(_sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, [d](const QModelIndex &parent, int start, int end) {
            d->sourceRowsAboutToBeInserted(parent, start, end);
        });

        connect(_sourceModel, &QAbstractItemModel::rowsInserted, this, [d](const QModelIndex &parent, int start, int end) {
            d->sourceRowsInserted(parent, start, end);
        });

        connect(_sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, [d](const QModelIndex &parent, int start, int end) {
            d->sourceRowsAboutToBeRemoved(parent, start, end);
        });

        connect(_sourceModel, &QAbstractItemModel::rowsRemoved, this, [d](const QModelIndex &parent, int start, int end) {
            d->sourceRowsRemoved(parent, start, end);
        });

        connect(_sourceModel,
                &QAbstractItemModel::rowsAboutToBeMoved,
                this,
                [d](const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destStart) {
                    d->sourceRowsAboutToBeMoved(srcParent, srcStart, srcEnd, destParent, destStart);
                });

        connect(_sourceModel,
                &QAbstractItemModel::rowsMoved,
                this,
                [d](const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destStart) {
                    d->sourceRowsMoved(srcParent, srcStart, srcEnd, destParent, destStart);
                });

        connect(_sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, [d]() {
            d->sourceModelAboutToBeReset();
        });

        connect(_sourceModel, &QAbstractItemModel::modelReset, this, [d]() {
            d->sourceModelReset();
        });

        connect(_sourceModel, &QAbstractItemModel::dataChanged, this, [d](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
            d->sourceDataChanged(topLeft, bottomRight);
        });

        connect(_sourceModel, &QAbstractItemModel::layoutAboutToBeChanged, this, [d]() {
            d->sourceLayoutAboutToBeChanged();
        });

        connect(_sourceModel, &QAbstractItemModel::layoutChanged, this, [d]() {
            d->sourceLayoutChanged();
        });

        connect(_sourceModel, &QObject::destroyed, this, [d]() {
            d->sourceModelDestroyed();
        });
    }

    resetInternalData();
    if (_sourceModel && _sourceModel->hasChildren()) {
        d->synchronousMappingRefresh();
    }

    endResetModel();
    Q_EMIT sourceModelChanged();
}

QModelIndex KDescendantsProxyModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

bool KDescendantsProxyModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const KDescendantsProxyModel);
    return !(d->m_mapping.isEmpty() || parent.isValid());
}

int KDescendantsProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const KDescendantsProxyModel);
    if (d->m_pendingParents.contains(parent) || parent.isValid() || !sourceModel()) {
        return 0;
    }

    if (d->m_mapping.isEmpty() && sourceModel()->hasChildren()) {
        const_cast<KDescendantsProxyModelPrivate *>(d)->synchronousMappingRefresh();
    }
    return d->m_rowCount;
}

QModelIndex KDescendantsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return QModelIndex();
    }

    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex KDescendantsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_D(const KDescendantsProxyModel);
    if (d->m_mapping.isEmpty() || !proxyIndex.isValid() || !sourceModel()) {
        return QModelIndex();
    }

    const Mapping::right_const_iterator result = d->m_mapping.rightLowerBound(proxyIndex.row());
    Q_ASSERT(result != d->m_mapping.rightEnd());

    const int proxyLastRow = result.key();
    const QModelIndex sourceLastChild = result.value();
    Q_ASSERT(sourceLastChild.isValid());

    // proxyLastRow is greater than proxyIndex.row().
    // sourceLastChild is vertically below the result we're looking for
    // and not necessarily in the correct parent.
    // We travel up through its parent hierarchy until we are in the
    // right parent, then return the correct sibling.

    // Source:           Proxy:    Row
    // - A               - A       - 0
    // - B               - B       - 1
    // - C               - C       - 2
    // - D               - D       - 3
    // - - E             - E       - 4
    // - - F             - F       - 5
    // - - G             - G       - 6
    // - - H             - H       - 7
    // - - I             - I       - 8
    // - - - J           - J       - 9
    // - - - K           - K       - 10
    // - - - L           - L       - 11
    // - - M             - M       - 12
    // - - N             - N       - 13
    // - O               - O       - 14

    // Note that L, N and O are lastChildIndexes, and therefore have a mapping. If we
    // are trying to map G from the proxy to the source, We at this point have an iterator
    // pointing to (L -> 11). The proxy row of G is 6. (proxyIndex.row() == 6). We seek the
    // sourceIndex which is vertically above L by the distance proxyLastRow - proxyIndex.row().
    // In this case the verticalDistance is 5.

    int verticalDistance = proxyLastRow - proxyIndex.row();

    // We traverse the ancestors of L, until we can index the desired row in the source.

    QModelIndex ancestor = sourceLastChild;
    while (ancestor.isValid()) {
        const int ancestorRow = ancestor.row();
        if (verticalDistance <= ancestorRow) {
            return ancestor.sibling(ancestorRow - verticalDistance, proxyIndex.column());
        }
        verticalDistance -= (ancestorRow + 1);
        ancestor = ancestor.parent();
    }
    Q_ASSERT(!"Didn't find target row.");
    return QModelIndex();
}

QModelIndex KDescendantsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_D(const KDescendantsProxyModel);

    if (!sourceModel()) {
        return QModelIndex();
    }

    if (d->m_mapping.isEmpty()) {
        return QModelIndex();
    }

    {
        // TODO: Consider a parent Mapping to speed this up.

        Mapping::right_const_iterator it = d->m_mapping.rightConstBegin();
        const Mapping::right_const_iterator end = d->m_mapping.rightConstEnd();
        const QModelIndex sourceParent = sourceIndex.parent();
        Mapping::right_const_iterator result = end;

        if (!isSourceIndexVisible(sourceIndex)) {
            return QModelIndex();
        }

        for (; it != end; ++it) {
            QModelIndex index = it.value();
            bool found_block = false;
            while (index.isValid()) {
                const QModelIndex ancestor = index.parent();
                if (ancestor == sourceParent && index.row() >= sourceIndex.row()) {
                    found_block = true;
                    if (result == end || it.key() < result.key()) {
                        result = it;
                        break; // Leave the while loop. index is still valid.
                    }
                }
                index = ancestor;
            }
            if (found_block && !index.isValid())
            // Looked through the ascendants of it.key() without finding sourceParent.
            // That means we've already got the result we need.
            {
                break;
            }
        }
        Q_ASSERT(result != end);
        const QModelIndex sourceLastChild = result.value();
        int proxyRow = result.key();
        QModelIndex index = sourceLastChild;
        while (index.isValid()) {
            const QModelIndex ancestor = index.parent();
            if (ancestor == sourceParent) {
                return createIndex(proxyRow - (index.row() - sourceIndex.row()), sourceIndex.column());
            }
            proxyRow -= (index.row() + 1);
            index = ancestor;
        }
        Q_ASSERT(!"Didn't find valid proxy mapping.");
        return QModelIndex();
    }
}

int KDescendantsProxyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() /* || rowCount(parent) == 0 */ || !sourceModel()) {
        return 0;
    }

    return sourceModel()->columnCount();
}

QVariant KDescendantsProxyModel::data(const QModelIndex &index, int role) const
{
    Q_D(const KDescendantsProxyModel);

    if (!sourceModel()) {
        return QVariant();
    }

    if (!index.isValid()) {
        return sourceModel()->data(index, role);
    }

    QModelIndex sourceIndex = mapToSource(index);

    if ((d->m_displayAncestorData) && (role == Qt::DisplayRole)) {
        if (!sourceIndex.isValid()) {
            return QVariant();
        }
        QString displayData = sourceIndex.data().toString();
        sourceIndex = sourceIndex.parent();
        while (sourceIndex.isValid()) {
            displayData.prepend(d->m_ancestorSeparator);
            displayData.prepend(sourceIndex.data().toString());
            sourceIndex = sourceIndex.parent();
        }
        return displayData;
    } else if (role == LevelRole) {
        QModelIndex sourceIndex = mapToSource(index);
        int level = 0;
        while (sourceIndex.isValid()) {
            sourceIndex = sourceIndex.parent();
            ++level;
        }
        return level;
    } else if (role == ExpandableRole) {
        QModelIndex sourceIndex = mapToSource(index);
        return sourceModel()->hasChildren(sourceIndex);
    } else if (role == ExpandedRole) {
        return isSourceIndexExpanded(mapToSource(index));
    } else if (role == HasSiblingsRole) {
        QModelIndex sourceIndex = mapToSource(index);
        QList<bool> hasSibling;
        while (sourceIndex.isValid()) {
            hasSibling.prepend(sourceModel()->rowCount(sourceIndex.parent()) > sourceIndex.row() + 1);
            sourceIndex = sourceIndex.parent();
        }
        return QVariant::fromValue(hasSibling);
    } else {
        return sourceIndex.data(role);
    }
}

QVariant KDescendantsProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!sourceModel() || columnCount() <= section) {
        return QVariant();
    }

    // Here is safe to do sourceModel()->headerData, as in this proxy we neither filter out nor reorder columns
    return sourceModel()->headerData(section, orientation, role);
}

Qt::ItemFlags KDescendantsProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || !sourceModel()) {
        return QAbstractProxyModel::flags(index);
    }

    const QModelIndex srcIndex = mapToSource(index);
    Q_ASSERT(srcIndex.isValid());
    return sourceModel()->flags(srcIndex);
}

void KDescendantsProxyModelPrivate::notifyhasSiblings(const QModelIndex &parent)
{
    Q_Q(KDescendantsProxyModel);

    if (!parent.isValid()) {
        return;
    }

    QModelIndex localParent = q->mapFromSource(parent);
    Q_EMIT q->dataChanged(localParent, localParent, {KDescendantsProxyModel::HasSiblingsRole});
    for (int i = 0; i < q->sourceModel()->rowCount(parent); ++i) {
        notifyhasSiblings(q->sourceModel()->index(i, 0, parent));
    }
}

void KDescendantsProxyModelPrivate::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_Q(KDescendantsProxyModel);

    if (parent.isValid() && (!q->isSourceIndexExpanded(parent) || !q->isSourceIndexVisible(parent))) {
        return;
    }

    if (!q->sourceModel()->hasChildren(parent)) {
        Q_ASSERT(q->sourceModel()->rowCount(parent) == 0);
        // parent was not a parent before.
        return;
    }

    int proxyStart = -1;

    const int rowCount = q->sourceModel()->rowCount(parent);

    if (rowCount > start) {
        const QModelIndex belowStart = q->sourceModel()->index(start, 0, parent);
        proxyStart = q->mapFromSource(belowStart).row();
    } else if (rowCount == 0) {
        proxyStart = q->mapFromSource(parent).row() + 1;
    } else {
        Q_ASSERT(rowCount == start);
        static const int column = 0;
        QModelIndex idx = q->sourceModel()->index(rowCount - 1, column, parent);
        while (q->isSourceIndexExpanded(idx) && q->sourceModel()->hasChildren(idx) && q->sourceModel()->rowCount(idx) > 0) {
            idx = q->sourceModel()->index(q->sourceModel()->rowCount(idx) - 1, column, idx);
        }
        // The last item in the list is getting a sibling below it.
        proxyStart = q->mapFromSource(idx).row() + 1;
    }
    const int proxyEnd = proxyStart + (end - start);

    m_insertPair = qMakePair(proxyStart, proxyEnd);
    q->beginInsertRows(QModelIndex(), proxyStart, proxyEnd);
}

void KDescendantsProxyModelPrivate::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_Q(KDescendantsProxyModel);
    if (parent.isValid() && (!q->isSourceIndexExpanded(parent) || !q->isSourceIndexVisible(parent))) {
        const QModelIndex index = q->mapFromSource(parent);
        Q_EMIT q->dataChanged(index,
                              index,
                              {KDescendantsProxyModel::ExpandableRole, KDescendantsProxyModel::ExpandedRole, KDescendantsProxyModel::HasSiblingsRole});
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }
    Q_ASSERT(q->sourceModel()->index(start, 0, parent).isValid());

    const int rowCount = q->sourceModel()->rowCount(parent);
    Q_ASSERT(rowCount > 0);

    const int difference = end - start + 1;

    if (rowCount == difference) {
        const QModelIndex index = q->mapFromSource(parent);
        if (parent.isValid()) {
            Q_EMIT q->dataChanged(index,
                                  index,
                                  {KDescendantsProxyModel::ExpandableRole, KDescendantsProxyModel::ExpandedRole, KDescendantsProxyModel::HasSiblingsRole});
        }
        // @p parent was not a parent before.
        m_pendingParents.append(parent);
        scheduleProcessPendingParents();
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }

    const int proxyStart = m_insertPair.first;

    Q_ASSERT(proxyStart >= 0);

    updateInternalIndexes(proxyStart, difference);

    if (rowCount - 1 == end) {
        // The previously last row (the mapped one) is no longer the last.
        // For example,

        // - A            - A           0
        // - - B          - B           1
        // - - C          - C           2
        // - - - D        - D           3
        // - - - E   ->   - E           4
        // - - F          - F           5
        // - - G     ->   - G           6
        // - H            - H           7
        // - I       ->   - I           8

        // As last children, E, F and G have mappings.
        // Consider that 'J' is appended to the children of 'C', below 'E'.

        // - A            - A           0
        // - - B          - B           1
        // - - C          - C           2
        // - - - D        - D           3
        // - - - E   ->   - E           4
        // - - - J        - ???         5
        // - - F          - F           6
        // - - G     ->   - G           7
        // - H            - H           8
        // - I       ->   - I           9

        // The updateInternalIndexes call above will have updated the F and G mappings correctly because proxyStart is 5.
        // That means that E -> 4 was not affected by the updateInternalIndexes call.
        // Now the mapping for E -> 4 needs to be updated so that it's a mapping for J -> 5.

        Q_ASSERT(!m_mapping.isEmpty());
        static const int column = 0;
        const QModelIndex oldIndex = q->sourceModel()->index(rowCount - 1 - difference, column, parent);
        Q_ASSERT(m_mapping.leftContains(oldIndex));

        const QModelIndex newIndex = q->sourceModel()->index(rowCount - 1, column, parent);

        QModelIndex indexAbove = oldIndex;

        if (start > 0) {
            // If we have something like this:
            //
            // - A
            // - - B
            // - - C
            //
            // and we then insert D as a sibling of A below it, we need to remove the mapping for A,
            // and the row number used for D must take into account the descendants of A.

            while (q->isSourceIndexExpanded(indexAbove) && q->sourceModel()->hasChildren(indexAbove)) {
                Q_ASSERT(q->sourceModel()->rowCount(indexAbove) > 0);
                indexAbove = q->sourceModel()->index(q->sourceModel()->rowCount(indexAbove) - 1, column, indexAbove);
            }
            Q_ASSERT(!q->isSourceIndexExpanded(indexAbove) || q->sourceModel()->rowCount(indexAbove) == 0);
        }

        Q_ASSERT(m_mapping.leftContains(indexAbove));

        const int newProxyRow = m_mapping.leftToRight(indexAbove) + difference;

        // oldIndex is E in the source. proxyRow is 4.
        m_mapping.removeLeft(oldIndex);

        // newIndex is J. (proxyRow + difference) is 5.
        m_mapping.insert(newIndex, newProxyRow);
    }

    for (int row = start; row <= end; ++row) {
        static const int column = 0;
        const QModelIndex idx = q->sourceModel()->index(row, column, parent);
        Q_ASSERT(idx.isValid());

        if (q->isSourceIndexExpanded(idx) && q->sourceModel()->hasChildren(idx) && q->sourceModel()->rowCount(idx) > 0) {
            m_pendingParents.append(idx);
        }
    }

    m_rowCount += difference;

    q->endInsertRows();
    scheduleProcessPendingParents();
    if (parent.isValid()) {
        const QModelIndex index = q->mapFromSource(parent);
        Q_EMIT q->dataChanged(index,
                              index,
                              {KDescendantsProxyModel::ExpandableRole, KDescendantsProxyModel::ExpandedRole, KDescendantsProxyModel::HasSiblingsRole});
    }

    if (start > 0) {
        notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
    }
}

void KDescendantsProxyModelPrivate::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_Q(KDescendantsProxyModel);

    if (!q->isSourceIndexExpanded(parent) || !q->isSourceIndexVisible(parent)) {
        return;
    }

    const int proxyStart = q->mapFromSource(q->sourceModel()->index(start, 0, parent)).row();

    static const int column = 0;
    QModelIndex idx = q->sourceModel()->index(end, column, parent);
    while (q->sourceModel()->hasChildren(idx) && q->sourceModel()->rowCount(idx) > 0) {
        idx = q->sourceModel()->index(q->sourceModel()->rowCount(idx) - 1, column, idx);
    }
    const int proxyEnd = q->mapFromSource(idx).row();

    for (int i = start; i <= end; ++i) {
        QModelIndex idx = q->sourceModel()->index(i, column, parent);
        m_expandedSourceIndexes.remove(QPersistentModelIndex(idx));
    }

    m_removePair = qMakePair(proxyStart, proxyEnd);

    q->beginRemoveRows(QModelIndex(), proxyStart, proxyEnd);
}

static QModelIndex getFirstDeepest(QAbstractItemModel *model, const QModelIndex &parent, int *count)
{
    static const int column = 0;
    Q_ASSERT(model->hasChildren(parent));
    Q_ASSERT(model->rowCount(parent) > 0);
    for (int row = 0; row < model->rowCount(parent); ++row) {
        (*count)++;
        const QModelIndex child = model->index(row, column, parent);
        Q_ASSERT(child.isValid());
        if (model->hasChildren(child)) {
            return getFirstDeepest(model, child, count);
        }
    }
    return model->index(model->rowCount(parent) - 1, column, parent);
}

void KDescendantsProxyModelPrivate::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_Q(KDescendantsProxyModel);
    Q_UNUSED(end)

    if (!q->isSourceIndexExpanded(parent) || !q->isSourceIndexVisible(parent)) {
        if (parent.isValid()) {
            const QModelIndex index = q->mapFromSource(parent);
            Q_EMIT q->dataChanged(index, index, {KDescendantsProxyModel::ExpandableRole, KDescendantsProxyModel::ExpandedRole});
        }
        return;
    }

    const int rowCount = q->sourceModel()->rowCount(parent);

    const int proxyStart = m_removePair.first;
    const int proxyEnd = m_removePair.second;

    const int difference = proxyEnd - proxyStart + 1;
    {
        Mapping::right_iterator it = m_mapping.rightLowerBound(proxyStart);
        const Mapping::right_iterator endIt = m_mapping.rightUpperBound(proxyEnd);

        if (endIt != m_mapping.rightEnd()) {
            while (it != endIt) {
                it = m_mapping.eraseRight(it);
            }
        } else {
            while (it != m_mapping.rightUpperBound(proxyEnd)) {
                it = m_mapping.eraseRight(it);
            }
        }
    }

    m_removePair = qMakePair(-1, -1);
    m_rowCount -= difference;
    Q_ASSERT(m_rowCount >= 0);

    updateInternalIndexes(proxyStart, -1 * difference);

    if (rowCount != start || rowCount == 0) {
        q->endRemoveRows();
        if (parent.isValid()) {
            const QModelIndex index = q->mapFromSource(parent);
            Q_EMIT q->dataChanged(index, index, {KDescendantsProxyModel::ExpandableRole, KDescendantsProxyModel::ExpandedRole});
        }
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }

    static const int column = 0;
    const QModelIndex newEnd = q->sourceModel()->index(rowCount - 1, column, parent);
    Q_ASSERT(newEnd.isValid());

    if (m_mapping.isEmpty()) {
        m_mapping.insert(newEnd, newEnd.row());
        q->endRemoveRows();
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }
    if (q->sourceModel()->hasChildren(newEnd)) {
        int count = 0;
        const QModelIndex firstDeepest = getFirstDeepest(q->sourceModel(), newEnd, &count);
        Q_ASSERT(firstDeepest.isValid());
        const int firstDeepestProxy = m_mapping.leftToRight(firstDeepest);

        m_mapping.insert(newEnd, firstDeepestProxy - count);
        q->endRemoveRows();
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }
    Mapping::right_iterator lowerBound = m_mapping.rightLowerBound(proxyStart);
    if (lowerBound == m_mapping.rightEnd()) {
        auto i = lowerBound;
        --i;
        int proxyRow = (i).key();

        for (int row = newEnd.row(); row >= 0; --row) {
            const QModelIndex newEndSibling = q->sourceModel()->index(row, column, parent);
            if (!q->sourceModel()->hasChildren(newEndSibling)) {
                ++proxyRow;
            } else {
                break;
            }
        }
        m_mapping.insert(newEnd, proxyRow);
        q->endRemoveRows();
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    } else if (lowerBound == m_mapping.rightBegin()) {
        int proxyRow = rowCount - 1;
        QModelIndex trackedParent = parent;
        while (trackedParent.isValid()) {
            proxyRow += (trackedParent.row() + 1);
            trackedParent = trackedParent.parent();
        }
        m_mapping.insert(newEnd, proxyRow);
        q->endRemoveRows();
        if (start > 0) {
            notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
        }
        return;
    }

    Mapping::right_iterator boundAbove = lowerBound;
    --boundAbove;

    QVector<QModelIndex> targetParents;
    targetParents.push_back(parent);
    {
        QModelIndex target = parent;
        int count = 0;
        while (target.isValid()) {
            if (target == boundAbove.value()) {
                m_mapping.insert(newEnd, count + boundAbove.key() + newEnd.row() + 1);
                q->endRemoveRows();
                if (start > 0) {
                    notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
                }
                return;
            }
            count += (target.row() + 1);
            target = target.parent();
            if (target.isValid()) {
                targetParents.push_back(target);
            }
        }
    }

    QModelIndex boundParent = boundAbove.value().parent();
    QModelIndex prevParent = boundParent;
    Q_ASSERT(boundParent.isValid());
    while (boundParent.isValid()) {
        prevParent = boundParent;
        boundParent = boundParent.parent();

        if (targetParents.contains(prevParent)) {
            break;
        }

        if (!m_mapping.leftContains(prevParent)) {
            break;
        }

        if (m_mapping.leftToRight(prevParent) > boundAbove.key()) {
            break;
        }
    }

    QModelIndex trackedParent = parent;

    int proxyRow = boundAbove.key();

    Q_ASSERT(prevParent.isValid());
    proxyRow -= prevParent.row();
    while (trackedParent != boundParent) {
        proxyRow += (trackedParent.row() + 1);
        trackedParent = trackedParent.parent();
    }
    m_mapping.insert(newEnd, proxyRow + newEnd.row());
    q->endRemoveRows();

    if (parent.isValid()) {
        const QModelIndex oindex = q->mapFromSource(parent);
        QVector<int> rolesChanged({KDescendantsProxyModel::ExpandableRole});

        if (!q->sourceModel()->hasChildren(parent)) {
            rolesChanged << KDescendantsProxyModel::ExpandedRole;
        } else if (q->sourceModel()->rowCount(parent) <= start) {
            const QModelIndex index = q->mapFromSource(q->sourceModel()->index(q->sourceModel()->rowCount(parent) - 1, 0, parent));
            Q_EMIT q->dataChanged(index, index, {KDescendantsProxyModel::ExpandedRole});
        }

        Q_EMIT q->dataChanged(oindex, oindex, rolesChanged);
    }

    if (start > 0) {
        notifyhasSiblings(q->sourceModel()->index(start - 1, 0, parent));
    }
}

void KDescendantsProxyModelPrivate::sourceRowsAboutToBeMoved(const QModelIndex &srcParent,
                                                             int srcStart,
                                                             int srcEnd,
                                                             const QModelIndex &destParent,
                                                             int destStart)
{
    Q_Q(KDescendantsProxyModel);

    Q_UNUSED(destStart)

    if (q->isSourceIndexExpanded(srcParent) && q->isSourceIndexVisible(srcParent)
        && (!q->isSourceIndexExpanded(destParent) || !q->isSourceIndexVisible(destParent))) {
        const QModelIndex proxySrcParent = q->mapFromSource(srcParent);
        const int proxyParentRow = proxySrcParent.isValid() ? proxySrcParent.row() : 0;
        q->beginRemoveRows(QModelIndex(), proxyParentRow + srcStart, proxyParentRow + srcEnd);

    } else if ((!q->isSourceIndexExpanded(srcParent) || !q->isSourceIndexVisible(srcParent)) && q->isSourceIndexExpanded(destParent)
               && q->isSourceIndexVisible(destParent)) {
        const QModelIndex proxyDestParent = q->mapFromSource(srcParent);
        const int proxyParentRow = proxyDestParent.isValid() ? proxyDestParent.row() : 0;

        q->beginInsertRows(QModelIndex(), proxyParentRow + destStart, proxyParentRow + destStart + (srcEnd - srcStart));
    }

    sourceLayoutAboutToBeChanged();
}

void KDescendantsProxyModelPrivate::sourceRowsMoved(const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destStart)
{
    Q_Q(KDescendantsProxyModel);

    Q_UNUSED(srcParent)
    Q_UNUSED(srcStart)
    Q_UNUSED(srcEnd)
    Q_UNUSED(destParent)
    Q_UNUSED(destStart)

    if (q->isSourceIndexExpanded(srcParent) && q->isSourceIndexVisible(srcParent)
        && (!q->isSourceIndexExpanded(destParent) || !q->isSourceIndexVisible(destParent))) {
        q->endRemoveRows();
    } else if (!q->isSourceIndexExpanded(srcParent) && q->isSourceIndexExpanded(destParent)) {
        q->endInsertRows();
    }

    sourceLayoutChanged();

    const QModelIndex index1 = q->mapFromSource(srcParent);
    const QModelIndex index2 = q->mapFromSource(destParent);
    Q_EMIT q->dataChanged(index1, index1, {KDescendantsProxyModel::ExpandableRole});
    if (index1 != index2) {
        Q_EMIT q->dataChanged(index2, index2, {KDescendantsProxyModel::ExpandableRole});
        if (!q->sourceModel()->hasChildren(destParent)) {
            Q_EMIT q->dataChanged(index2, index2, {KDescendantsProxyModel::ExpandableRole});
        }
    }
    const QModelIndex lastIndex = q->mapFromSource(q->sourceModel()->index(q->sourceModel()->rowCount(srcParent) - 1, 0, srcParent));
    Q_EMIT q->dataChanged(lastIndex, lastIndex, {KDescendantsProxyModel::ExpandableRole});

    if (srcStart > 0) {
        notifyhasSiblings(q->sourceModel()->index(srcStart - 1, 0, srcParent));
    }
    if (destStart > 0) {
        notifyhasSiblings(q->sourceModel()->index(destStart - 1, 0, destParent));
    }
}

void KDescendantsProxyModelPrivate::sourceModelAboutToBeReset()
{
    Q_Q(KDescendantsProxyModel);
    q->beginResetModel();
}

void KDescendantsProxyModelPrivate::sourceModelReset()
{
    Q_Q(KDescendantsProxyModel);
    resetInternalData();
    if (q->sourceModel()->hasChildren() && q->sourceModel()->rowCount() > 0) {
        m_pendingParents.append(QModelIndex());
        scheduleProcessPendingParents();
    }
    q->endResetModel();
}

void KDescendantsProxyModelPrivate::sourceLayoutAboutToBeChanged()
{
    Q_Q(KDescendantsProxyModel);

    if (m_ignoreNextLayoutChanged) {
        m_ignoreNextLayoutChanged = false;
        return;
    }

    if (m_mapping.isEmpty()) {
        return;
    }

    Q_EMIT q->layoutAboutToBeChanged();

    QPersistentModelIndex srcPersistentIndex;
    const auto lst = q->persistentIndexList();
    for (const QModelIndex &proxyPersistentIndex : lst) {
        m_proxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        m_layoutChangePersistentIndexes << srcPersistentIndex;
    }
}

void KDescendantsProxyModelPrivate::sourceLayoutChanged()
{
    Q_Q(KDescendantsProxyModel);

    if (m_ignoreNextLayoutAboutToBeChanged) {
        m_ignoreNextLayoutAboutToBeChanged = false;
        return;
    }

    if (m_mapping.isEmpty()) {
        return;
    }

    m_rowCount = 0;

    synchronousMappingRefresh();

    for (int i = 0; i < m_proxyIndexes.size(); ++i) {
        q->changePersistentIndex(m_proxyIndexes.at(i), q->mapFromSource(m_layoutChangePersistentIndexes.at(i)));
    }

    m_layoutChangePersistentIndexes.clear();
    m_proxyIndexes.clear();

    Q_EMIT q->layoutChanged();
}

void KDescendantsProxyModelPrivate::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_Q(KDescendantsProxyModel);
    // It is actually possible in a real world scenario that the source model emits dataChanged
    // with invalid indexes when the source model is a QSortFilterProxyModel
    // because QSortFilterProxyModel doesn't check for mapped index validity when its
    // source model emitted dataChanged on a column QSortFilterProxyModel doesn't accept.
    // See https://bugreports.qt.io/browse/QTBUG-86850
    if (!topLeft.isValid() || !bottomRight.isValid()) {
        return;
    }
    Q_ASSERT(topLeft.model() == q->sourceModel());
    Q_ASSERT(bottomRight.model() == q->sourceModel());

    if (!q->isSourceIndexExpanded(topLeft.parent()) || !q->isSourceIndexVisible(topLeft.parent())) {
        return;
    }

    const int topRow = topLeft.row();
    const int bottomRow = bottomRight.row();

    for (int i = topRow; i <= bottomRow; ++i) {
        const QModelIndex sourceTopLeft = q->sourceModel()->index(i, topLeft.column(), topLeft.parent());

        Q_ASSERT(sourceTopLeft.isValid());
        const QModelIndex proxyTopLeft = q->mapFromSource(sourceTopLeft);
        // TODO. If an index does not have any descendants, then we can emit in blocks of rows.
        // As it is we emit once for each row.
        const QModelIndex sourceBottomRight = q->sourceModel()->index(i, bottomRight.column(), bottomRight.parent());
        const QModelIndex proxyBottomRight = q->mapFromSource(sourceBottomRight);
        Q_ASSERT(proxyTopLeft.isValid());
        Q_ASSERT(proxyBottomRight.isValid());
        Q_EMIT q->dataChanged(proxyTopLeft, proxyBottomRight);
    }
}

void KDescendantsProxyModelPrivate::sourceModelDestroyed()
{
    resetInternalData();
}

QMimeData *KDescendantsProxyModel::mimeData(const QModelIndexList &indexes) const
{
    if (!sourceModel()) {
        return QAbstractProxyModel::mimeData(indexes);
    }
    Q_ASSERT(sourceModel());
    QModelIndexList sourceIndexes;
    for (const QModelIndex &index : indexes) {
        sourceIndexes << mapToSource(index);
    }
    return sourceModel()->mimeData(sourceIndexes);
}

QStringList KDescendantsProxyModel::mimeTypes() const
{
    if (!sourceModel()) {
        return QAbstractProxyModel::mimeTypes();
    }
    Q_ASSERT(sourceModel());
    return sourceModel()->mimeTypes();
}

Qt::DropActions KDescendantsProxyModel::supportedDropActions() const
{
    if (!sourceModel()) {
        return QAbstractProxyModel::supportedDropActions();
    }
    return sourceModel()->supportedDropActions();
}

#include "moc_kdescendantsproxymodel.cpp"
