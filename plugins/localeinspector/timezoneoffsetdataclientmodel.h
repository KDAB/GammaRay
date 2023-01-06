/*
  timezoneoffsetdataclientmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TIMEZONEOFFSETDATACLIENTMODEL_H
#define GAMMARAY_TIMEZONEOFFSETDATACLIENTMODEL_H

#include <QIdentityProxyModel>

namespace GammaRay {

class TimezoneOffsetDataClientModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit TimezoneOffsetDataClientModel(QObject *parent = nullptr);
    ~TimezoneOffsetDataClientModel() override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

}

#endif // GAMMARAY_TIMEZONEOFFSETDATACLIENTMODEL_H
