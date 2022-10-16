/*
  qt3dentitytreemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QT3DENTITYTREEMODEL_H
#define GAMMARAY_QT3DENTITYTREEMODEL_H

#include <core/objectmodelbase.h>

#include <QHash>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Qt3DCore {
class QAspectEngine;
class QEntity;
class QNode;
}
QT_END_NAMESPACE

namespace GammaRay {
/** Model for the entity tree of an QAspectEngine. */
class Qt3DEntityTreeModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit Qt3DEntityTreeModel(QObject *parent = nullptr);
    ~Qt3DEntityTreeModel();

    void setEngine(Qt3DCore::QAspectEngine *engine);

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
    void populateFromNode(Qt3DCore::QNode *node);
    void populateFromEntity(Qt3DCore::QEntity *entity);
    void removeEntity(Qt3DCore::QEntity *entity, bool danglingPointer);
    void removeSubtree(Qt3DCore::QEntity *entity, bool danglingPointer);
    QModelIndex indexForEntity(Qt3DCore::QEntity *entity) const;

    void connectEntity(Qt3DCore::QEntity *entity) const;
    void disconnectEntity(Qt3DCore::QEntity *entity) const;
    void entityEnabledChanged();

private:
    Qt3DCore::QAspectEngine *m_engine;

    QHash<Qt3DCore::QEntity *, Qt3DCore::QEntity *> m_childParentMap;
    QHash<Qt3DCore::QEntity *, QVector<Qt3DCore::QEntity *>> m_parentChildMap;
};
}

#endif // GAMMARAY_QT3DENTITYTREEMODEL_H
