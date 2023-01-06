/*
  quickscenegraphmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEGRAPHMODEL_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEGRAPHMODEL_H

#include <config-gammaray.h>

#include "core/objectmodelbase.h"

#include <QHash>
#include <QPointer>
#include <QVector>
#include <unordered_map>

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
    std::unordered_map<QSGNode *, QSGNode *> m_childParentMap;
    std::unordered_map<QSGNode *, QVector<QSGNode *>> m_parentChildMap;
    std::unordered_map<QQuickItem *, QSGNode *> m_itemItemNodeMap;
    std::unordered_map<QSGNode *, QQuickItem *> m_itemNodeItemMap;
};
}

#endif // GAMMARAY_QUICKSCENEGRAPHMODEL_H
