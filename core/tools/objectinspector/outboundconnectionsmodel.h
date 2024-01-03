/*
  outboundconnectionsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_OUTBOUNDCONNECTIONSMODEL_H
#define GAMMARAY_OBJECTINSPECTOR_OUTBOUNDCONNECTIONSMODEL_H

#include "abstractconnectionsmodel.h"

namespace GammaRay {
/** Lists outgoing connections from a given QObject. */
class OutboundConnectionsModel : public AbstractConnectionsModel
{
    Q_OBJECT

public:
    explicit OutboundConnectionsModel(QObject *parent = nullptr);
    ~OutboundConnectionsModel() override;

    void setObject(QObject *object) override;

    static QVector<Connection> outboundConnectionsForObject(QObject *object);

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_OUTBOUNDCONNECTIONMODEL_H
