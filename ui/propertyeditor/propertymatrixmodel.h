/*
  propertymatrixmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYMATRIXMODEL_H
#define GAMMARAY_PROPERTYMATRIXMODEL_H

#include <QAbstractTableModel>

namespace GammaRay {
class PropertyMatrixModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PropertyMatrixModel(QObject *parent = nullptr);

    void setMatrix(const QVariant &matrix);
    QVariant matrix() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &data,
                 int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QVariant m_matrix;
};
}

#endif // GAMMARAY_PROPERTYMATRIXMODEL_H
