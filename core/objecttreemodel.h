/*
  objecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTTREEMODEL_H
#define GAMMARAY_OBJECTTREEMODEL_H

#include "objectmodelbase.h"

#include <QVector>

namespace GammaRay {
class Probe;

class ObjectTreeModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit ObjectTreeModel(Probe *probe);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE static QPair<int, QVariant> defaultSelectedItem();

private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectReparented(QObject *obj);
    void objectFavorited(QObject *obj);
    void objectUnfavorited(QObject *obj);

private:
    QModelIndex indexForObject(QObject *object) const;

private:
    QHash<QObject *, QObject *> m_childParentMap;
    QHash<QObject *, QVector<QObject *>> m_parentChildMap;
    QSet<QObject *> m_favorites;
};
}

#endif // GAMMARAY_OBJECTTREEMODEL_H
