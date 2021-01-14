/*
  quickscenegraphmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEGRAPHMODEL_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEGRAPHMODEL_H

#include <config-gammaray.h>

#include "core/objectmodelbase.h"

#include <QHash>
#include <QPointer>
#include <QVector>

QT_BEGIN_NAMESPACE
class QSGNode;
class QQuickItem;
class QQuickWindow;
QT_END_NAMESPACE

namespace GammaRay {
/** QQ2 scene graph model. */
class QuickSceneGraphModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit QuickSceneGraphModel(QObject *parent = nullptr);
    ~QuickSceneGraphModel() override;

    void setWindow(QQuickWindow *window);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex indexForNode(QSGNode *node) const;
    QSGNode *sgNodeForItem(QQuickItem *item) const;
    QQuickItem *itemForSgNode(QSGNode *node) const;
    bool verifyNodeValidity(QSGNode *node);

signals:
    void nodeDeleted(QSGNode *node);

private slots:
    void updateSGTree(bool emitSignals = true);

private:
    void clear();
    QSGNode *currentRootNode() const;
    void populateFromNode(QSGNode *node, bool emitSignals);
    void collectItemNodes(QQuickItem *item);
    bool recursivelyFindChild(QSGNode *root, QSGNode *child) const;
    void pruneSubTree(QSGNode *node);

    QPointer<QQuickWindow> m_window;

    QSGNode *m_rootNode;
    QHash<QSGNode *, QSGNode *> m_childParentMap;
    QHash<QSGNode *, QVector<QSGNode *> > m_parentChildMap;
    QHash<QQuickItem *, QSGNode *> m_itemItemNodeMap;
    QHash<QSGNode *, QQuickItem *> m_itemNodeItemMap;
};
}

#endif // GAMMARAY_QUICKSCENEGRAPHMODEL_H
