/*
  messagedisplaymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MESSAGEDISPLAYMODEL_H
#define GAMMARAY_MESSAGEDISPLAYMODEL_H

#include <QIdentityProxyModel>

namespace GammaRay {
/** Client side additions to the message model. */
class MessageDisplayModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit MessageDisplayModel(QObject *parent = nullptr);
    ~MessageDisplayModel() override;

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_MESSAGEDISPLAYMODEL_H
