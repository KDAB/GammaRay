/*
  problemclientmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBLEMCLIENTMODEL_H
#define GAMMARAY_PROBLEMCLIENTMODEL_H

#include <QVector>
#include <QSortFilterProxyModel>

namespace GammaRay {

/*! Client-side part of the meta types model. */
class ProblemClientModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ProblemClientModel(QObject *parent = nullptr);
    ~ProblemClientModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void disableChecker(const QString &id);
    void enableChecker(const QString &id);

private:
    QVector<QString> m_disabledCheckers; // holds the ids of the disabled checkers
};

}

#endif // GAMMARAY_PROBLEMCLIENTMODEL_H
