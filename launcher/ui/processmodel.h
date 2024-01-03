/*
  processmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROCESSMODEL_H
#define GAMMARAY_PROCESSMODEL_H

#include <launcher/core/probeabi.h>

#include <QAbstractTableModel>
#include <QVector>

#include "processlist.h"

namespace GammaRay {
class ProcessModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ProcessModel(QObject *parent = nullptr);
    ~ProcessModel() override;

    void setProcesses(const ProcDataList &processes);
    void mergeProcesses(const ProcDataList &processes);
    ProcData dataForIndex(const QModelIndex &index) const;
    ProcData dataForRow(int row) const;
    QModelIndex indexForPid(const QString &pid) const;

    ProcDataList processes() const;

    void clear();

    enum Columns
    {
        PIDColumn,
        NameColumn,
        StateColumn,
        UserColumn,
        COLUMN_COUNT
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    enum CustomRoles
    {
        PIDRole = Qt::UserRole,
        NameRole,
        StateRole,
        UserRole,
        ABIRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    ProcDataList m_data;
    QVector<ProbeABI> m_availableABIs;
};
}

#endif // GAMMARAY_PROCESSMODEL_H
