/*
  transitionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_STATEMACHINEVIEWER_TRANSITIONMODEL_H
#define GAMMARAY_STATEMACHINEVIEWER_TRANSITIONMODEL_H

#include <core/objectmodelbase.h>

QT_BEGIN_NAMESPACE
class QAbstractState;
QT_END_NAMESPACE

namespace GammaRay {
class TransitionModelPrivate;

class TransitionModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit TransitionModel(QObject *parent = nullptr);
    ~TransitionModel() override;
    void setState(QAbstractState *state);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

protected:
    Q_DECLARE_PRIVATE(TransitionModel)
    TransitionModelPrivate *const d_ptr;
};
}

#endif
