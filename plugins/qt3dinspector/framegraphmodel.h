/*
  framegraphmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_FRAMEGRAPHMODEL_H
#define GAMMARAY_FRAMEGRAPHMODEL_H

#include <core/objectmodelbase.h>

#include <QAbstractListModel>
#include <QHash>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QFrameGraphNode;
class QRenderSettings;
}
QT_END_NAMESPACE

namespace GammaRay {
class FrameGraphModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit FrameGraphModel(QObject *parent = nullptr);
    ~FrameGraphModel();

    void setRenderSettings(Qt3DRender::QRenderSettings *settings);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public slots:
    void objectCreated(QObject *obj);
    void objectDestroyed(QObject *obj);
    void objectReparented(QObject *obj);

private:
    void clear();
    void populateFromNode(Qt3DRender::QFrameGraphNode *node);
    void removeNode(Qt3DRender::QFrameGraphNode *node, bool danglingPointer);
    void removeSubtree(Qt3DRender::QFrameGraphNode *node, bool danglingPointer);
    QModelIndex indexForNode(Qt3DRender::QFrameGraphNode *node) const;

    void connectNode(Qt3DRender::QFrameGraphNode *node);
    void disconnectNode(Qt3DRender::QFrameGraphNode *node);
    void nodeEnabledChanged();

private:
    Qt3DRender::QRenderSettings *m_settings;
    QHash<Qt3DRender::QFrameGraphNode *, Qt3DRender::QFrameGraphNode *> m_childParentMap;
    QHash<Qt3DRender::QFrameGraphNode *, QVector<Qt3DRender::QFrameGraphNode *> > m_parentChildMap;
};
}

#endif // GAMMARAY_FRAMEGRAPHMODEL_H
