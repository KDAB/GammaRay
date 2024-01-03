/*
  selectionmodelclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
