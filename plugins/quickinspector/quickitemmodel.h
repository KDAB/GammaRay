/*
  quickitemmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include <QVector>

class QSignalMapper;
class QQuickItem;
class QQuickWindow;

namespace GammaRay {

/** QQ2 item tree model. */
class QuickItemModel : public ObjectModelBase<QAbstractItemModel>
{
  Q_OBJECT

  public:
    explicit QuickItemModel(QObject *parent = 0);
    ~QuickItemModel();

    void setWindow(QQuickWindow *window);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QMap< int, QVariant > itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;

  public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

  private slots:
    void itemReparented();
    void itemWindowChanged();
    void itemUpdated();

  private:
    friend class QuickEventMonitor;
    void updateItem(QQuickItem *item);
    void recursivelyUpdateItem(QQuickItem *item);
    void updateItemFlags(QQuickItem *item);
    void clear();
    void populateFromItem(QQuickItem *item);

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

    QHash<QQuickItem*, QQuickItem*> m_childParentMap;
    QHash<QQuickItem*, QVector<QQuickItem*> > m_parentChildMap;
    QHash<QQuickItem*, int> m_itemFlags;
};

class QuickEventMonitor : public QObject
{
  Q_OBJECT
  public:
    explicit QuickEventMonitor(QuickItemModel *parent);

  protected:
    bool eventFilter(QObject *obj, QEvent *event);

  private:
    QuickItemModel *m_model;
};

}

#endif // GAMMARAY_QUICKITEMMODEL_H
