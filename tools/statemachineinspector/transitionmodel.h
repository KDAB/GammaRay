/*
  transitionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TRANSITIONMODEL_H
#define TRANSITIONMODEL_H

#include "objectmodelbase.h"

class QState;

namespace GammaRay
{

class TransitionModelPrivate;

class TransitionModel : public ObjectModelBase<QAbstractItemModel>
{
  public:
    TransitionModel(QObject *parent = 0);
    void setState(QState *state);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

  protected:
    Q_DECLARE_PRIVATE(TransitionModel)
    TransitionModelPrivate *const d_ptr;
};

}

#endif
