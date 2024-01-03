/*
  clientmethodmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTMETHODMODEL_H
#define GAMMARAY_CLIENTMETHODMODEL_H

#include <QIdentityProxyModel>

namespace GammaRay {

/*! Client side part of the QMetaMethod model. */
class ClientMethodModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit ClientMethodModel(QObject *parent = nullptr);
    ~ClientMethodModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

}

#endif // GAMMARAY_CLIENTMETHODMODEL_H
