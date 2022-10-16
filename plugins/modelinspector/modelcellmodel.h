/*
  modelcellmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MODELINSPECTOR_MODELCELLMODEL_H
#define GAMMARAY_MODELINSPECTOR_MODELCELLMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {
class ModelCellModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelCellModel(QObject *parent = nullptr);
    void setModelIndex(const QModelIndex &idx);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    typedef QPair<int, QString> RoleInfo;
    static QVector<RoleInfo> rolesForModel(const QAbstractItemModel *model);

    QPersistentModelIndex m_index;
    QVector<RoleInfo> m_roles;
};
}

#endif // GAMMARAY_MODELCELLMODEL_H
