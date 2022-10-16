/*
  availablecheckersmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_AVAILABLECHECKERSMODEL_H
#define GAMMARAY_AVAILABLECHECKERSMODEL_H

#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <core/problemcollector.h>

namespace GammaRay {

class AvailableCheckersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AvailableCheckersModel(QObject *parent);

    QVariant data(const QModelIndex &index, int role) const override;

    int rowCount(const QModelIndex &parent) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private slots:
    void aboutToAddChecker();
    void checkerAdded();

private:
    QVector<ProblemCollector::Checker> *m_availableCheckers;
};

}

#endif // GAMMARAY_AVAILABLECHECKERSMODEL_H
