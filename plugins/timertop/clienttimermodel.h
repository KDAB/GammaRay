/*
  clienttimermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTTIMERMODEL_H
#define GAMMARAY_CLIENTTIMERMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {

class ClientTimerModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ClientTimerModel(QObject *parent = nullptr);
    ~ClientTimerModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    static QString stateToString(int state, int interval);
    static QString wakeupsPerSecToString(qreal value);
    static QString timePerWakeupToString(qreal value);
    static QString maxWakeupTimeToString(uint value);
};

}

#endif // GAMMARAY_CLIENTTIMERMODEL_H
