/*
  widget3dview.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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
    explicit Widget3DWindowModel(QObject *parent = Q_NULLPTR);
    ~Widget3DWindowModel();

    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex & parent) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void sourceModelRowsInserted(const QModelIndex &parent, int first, int last);
    void sourceModelRowsRemoved();
    void sourceModelReset();

private:
    class WindowNode;

    void populate();
    QModelIndex indexForNode(WindowNode *node) const;

    QVector<WindowNode*> mNodes;
};

}

#endif
