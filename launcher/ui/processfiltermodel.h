/*
  processfiltermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROCESSFILTERMODEL_H
#define GAMMARAY_PROCESSFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {
// A filterable and sortable process model
class ProcessFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ProcessFilterModel(QObject *parent);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column,
                             const QModelIndex &source_parent) const override;

private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    qint64 m_currentProcId;
    QString m_currentUser;
};
}

#endif // GAMMARAY_PROCESSFILTERMODEL_H
