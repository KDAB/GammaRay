/*
  clientpropertymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTPROPERTYMODEL_H
#define GAMMARAY_CLIENTPROPERTYMODEL_H

#include "gammaray_ui_export.h"

#include <QIdentityProxyModel>

namespace GammaRay {

/*! Client-side proxy model for the AggregatedPropertyModel. */
class GAMMARAY_UI_EXPORT ClientPropertyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit ClientPropertyModel(QObject *parent = nullptr);
    ~ClientPropertyModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

}

#endif // GAMMARAY_CLIENTPROPERTYMODEL_H
