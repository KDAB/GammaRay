/*
  availablecheckersmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    QVariant data(const QModelIndex & index, int role) const override;

    int rowCount(const QModelIndex & parent) const override;

    bool setData(const QModelIndex & index, const QVariant & value, int role) override;

    Qt::ItemFlags flags(const QModelIndex & index) const override;

private slots:
    void aboutToAddChecker();
    void checkerAdded();

private:
    QVector<ProblemCollector::Checker> *m_availableCheckers;
};

}

#endif // GAMMARAY_AVAILABLECHECKERSMODEL_H
