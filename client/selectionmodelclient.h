/*
  selectionmodelclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SELECTIONMODELCLIENT_H
#define GAMMARAY_SELECTIONMODELCLIENT_H

#include <common/networkselectionmodel.h>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
/** Client side of the network transparent QItemSelectionModel. */
class SelectionModelClient : public NetworkSelectionModel
{
    Q_OBJECT
public:
    SelectionModelClient(const QString &objectName, QAbstractItemModel *model, QObject *parent);
    ~SelectionModelClient() override;

private slots:
    void timeout();
    void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void serverUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

private:
    void connectToServer();

    QTimer *m_timer;
};
}

#endif // GAMMARAY_SELECTIONMODELCLIENT_H
