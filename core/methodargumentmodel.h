/*
  methodargumentmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METHODARGUMENTMODEL_H
#define GAMMARAY_METHODARGUMENTMODEL_H

#include <common/methodargument.h>

#include <QAbstractTableModel>
#include <QMetaMethod>
#include <QVector>

namespace GammaRay {
class MethodArgumentModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MethodArgumentModel(QObject *parent = nullptr);
    void setMethod(const QMetaMethod &method);
    QVector<MethodArgument> arguments() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QMetaMethod m_method;
    QVector<QVariant> m_arguments;
};
}

#endif // GAMMARAY_METHODARGUMENTMODEL_H
