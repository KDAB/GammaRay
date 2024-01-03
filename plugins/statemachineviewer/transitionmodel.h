/*
  transitionmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
