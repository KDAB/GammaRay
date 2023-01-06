/*
  inboundconnectionsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_INBOUNDCONNECTIONSMODEL_H
#define GAMMARAY_OBJECTINSPECTOR_INBOUNDCONNECTIONSMODEL_H

#include "abstractconnectionsmodel.h"

namespace GammaRay {
/** List of inbound connections on a given QObject. */
class InboundConnectionsModel : public AbstractConnectionsModel
{
    Q_OBJECT
public:
    explicit InboundConnectionsModel(QObject *parent = nullptr);
    ~InboundConnectionsModel() override;

    void setObject(QObject *object) override;

    static QVector<Connection> inboundConnectionsForObject(QObject *object);

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_INBOUNDCONNECTIONSMODEL_H
