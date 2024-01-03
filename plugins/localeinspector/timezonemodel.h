/*
  timezonemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TIMEZONEMODEL_H
#define GAMMARAY_TIMEZONEMODEL_H

#include <QAbstractTableModel>
#include <QByteArray>
#include <QList>

namespace GammaRay {

class TimezoneModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TimezoneModel(QObject *parent = nullptr);
    ~TimezoneModel() override;

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    mutable QList<QByteArray> m_ids;
};

}

#endif // GAMMARAY_TIMEZONEMODEL_H
