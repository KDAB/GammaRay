/*
  quickitemmodel.h

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMMODEL_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMMODEL_H

#include <core/objectmodelbase.h>

#include <QHash>
#include <QPointer>
#include <QTimer>
#include <QVector>

#include <array>
#include <unordered_map>
#include <vector>

QT_BEGIN_NAMESPACE
class QQuickItem;
class QQuickWindow;
QT_END_NAMESPACE

namespace GammaRay {

//forward
class QuickEventMonitor;

/** QQ2 item tree model. */
class QuickItemModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT

public:
    explicit QuickItemModel(QObject *parent = nullptr);
    ~QuickItemModel() override;

    void setWindow(QQuickWindow *window);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QMap< int, QVariant > itemData(const QModelIndex &index) const override;

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

private slots:
    void itemReparented(QQuickItem *item);
    void itemWindowChanged(QQuickItem *item);
    void itemUpdated(QQuickItem *item);

private:
    friend class QuickEventMonitor;
    void updateItem(QQuickItem *item, int role);
    void recursivelyUpdateItem(QQuickItem *item);
    void updateItemFlags(QQuickItem *item);
    void clear();
    void populateFromItem(QQuickItem *item);

    /**
     * Reports problems (e.g. visible but out of view) about all items of this
     * model. Uses the item flags from the model.
     */
    void reportProblems();

    /// Track all changes to item @p item in this model (parentChanged, windowChanged, ...)
    void connectItem(QQuickItem *item);

    /// Untrack item @p item
    void disconnectItem(QQuickItem *item);
    QModelIndex indexForItem(QQuickItem *item) const;

    /// Add item @p item to this model
    void addItem(QQuickItem *item);

    /// Remove item @p item from this model.
    /// Set @p danglingPointer to true if the item has already been destructed
    void removeItem(QQuickItem *item, bool danglingPointer = false);

    /**
     * Remove item @p item from the internal data set.
     * This function won't cause rowsRemoved to be emitted.
     * Set @p danglingPointer to true if the item has already been destructed.
     */
    void doRemoveSubtree(QQuickItem *item, bool danglingPointer = false);

    QPointer<QQuickWindow> m_window;

    QHash<QQuickItem *, QQuickItem *> m_childParentMap;
    QHash<QQuickItem *, QVector<QQuickItem *> > m_parentChildMap;

    // TODO: Merge these two?
    QHash<QQuickItem *, int> m_itemFlags;
    std::unordered_map<QQuickItem *, std::array<QMetaObject::Connection, 8>> m_itemConnections;

    // dataChange signal compression
    struct PendingDataChange {
        QQuickItem *item = nullptr;
        bool eventChange = false;
        bool flagChange = false;
        inline bool operator<(QQuickItem *rhs) const { return item < rhs; }
    };
    std::vector<PendingDataChange> m_pendingDataChanges;
    QTimer *m_dataChangeTimer = nullptr;
    void emitPendingDataChanges();

    QuickEventMonitor *m_clickEventFilter;
};

class QuickEventMonitor : public QObject
{
    Q_OBJECT
public:
    explicit QuickEventMonitor(QuickItemModel *parent);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QuickItemModel *m_model;
};
}

#endif // GAMMARAY_QUICKITEMMODEL_H
