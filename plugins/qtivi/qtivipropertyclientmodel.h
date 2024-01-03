/*
  qtivipropertyclientmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QTIVI_PROPERTYCLIENTMODEL_H
#define GAMMARAY_QTIVI_PROPERTYCLIENTMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {
/** UI-dependent (and thus client-side) bits of the ivi property model. */
class QtIviPropertyClientModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QtIviPropertyClientModel(QObject *parent = nullptr);
    ~QtIviPropertyClientModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_QTIVI_PROPERTYCLIENTMODEL_H
