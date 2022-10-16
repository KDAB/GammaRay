/*
  widget3dwindowmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef WIDGET3DWINDOWMODEL_H
#define WIDGET3DWINDOWMODEL_H

#include <QAbstractProxyModel>
#include <QPersistentModelIndex>

#include "widget3dmodel.h"

namespace GammaRay {

class Widget3DWindowModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit Widget3DWindowModel(QObject *parent = nullptr);
    ~Widget3DWindowModel();

    void setSourceModel(QAbstractItemModel *sourceModel_) override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

private Q_SLOTS:
    void sourceModelRowsInserted(const QModelIndex &parent, int first, int last);
    void sourceModelRowsRemoved();
    void sourceModelReset();

private:
    class WindowNode;

    void populate();
    QModelIndex indexForNode(WindowNode *node) const;

    QVector<WindowNode *> mNodes;
};

}

#endif
