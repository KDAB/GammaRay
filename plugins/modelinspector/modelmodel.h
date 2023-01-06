/*
  modelmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MODELINSPECTOR_MODELMODEL_H
#define GAMMARAY_MODELINSPECTOR_MODELMODEL_H

#include <core/objectmodelbase.h>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QVector>

namespace GammaRay {
class ModelModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit ModelModel(QObject *parent);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

private:
    QModelIndex indexForModel(QAbstractItemModel *model) const;
    QVector<QAbstractProxyModel *> proxiesForModel(QAbstractItemModel *model) const;

private:
    QVector<QAbstractItemModel *> m_models;
    QVector<QAbstractProxyModel *> m_proxies;
};
}

#endif // MODELMODEL_H
