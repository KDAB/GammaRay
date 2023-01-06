/*
  problemmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBLEMMODEL_H
#define GAMMARAY_PROBLEMMODEL_H

#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <common/objectmodel.h>

namespace GammaRay {

class ProblemCollector;

/*! Model of all selectable client tools. */
class ProblemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProblemModel(QObject *parent);
    ~ProblemModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;

private slots:
    void aboutToAddProblem(int row);
    void problemAdded();
    void aboutToRemoveProblems(int row, int count = 1);
    void problemsRemoved();

private:
    ProblemCollector *m_problemCollector;
};

/*! Selection model that automatically syncs ClientToolModel with ClientToolManager. */
// class ClientToolSelectionModel : public QItemSelectionModel
// {
//     Q_OBJECT
// public:
//     explicit ClientToolSelectionModel(ClientToolManager *manager);
//     ~ClientToolSelectionModel();
//
// private slots:
//     void selectTool(int index);
//     void selectDefaultTool();
//
// private:
//     ClientToolManager *m_toolManager;
// };

}

#endif // GAMMARAY_PROBLEMMODEL_H
