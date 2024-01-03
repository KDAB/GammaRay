/*
  metaobjecttreeclientproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METAOBJECTTREECLIENTPROXYMODEL_H
#define GAMMARAY_METAOBJECTTREECLIENTPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QPersistentModelIndex>

namespace GammaRay {
/** Colors the usage counts based on the global ratio. */
class MetaObjectTreeClientProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit MetaObjectTreeClientProxyModel(QObject *parent = nullptr);
    ~MetaObjectTreeClientProxyModel() override;

    void setSourceModel(QAbstractItemModel *source) override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private slots:
    void findQObjectIndex();

private:
    bool needsBackground(const QModelIndex &index) const;

    QPersistentModelIndex m_qobjIndex;
};
}

#endif // GAMMARAY_METAOBJECTTREECLIENTPROXYMODEL_H
