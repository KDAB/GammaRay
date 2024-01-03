/*
  selectionmodelserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SELECTIONMODELSERVER_H
#define GAMMARAY_SELECTIONMODELSERVER_H

#include <common/networkselectionmodel.h>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
/** Server-side of the network transparent QItemSelection model. */
class SelectionModelServer : public NetworkSelectionModel
{
    Q_OBJECT
public:
    explicit SelectionModelServer(const QString &objectName, QAbstractItemModel *model,
                                  QObject *parent);
    ~SelectionModelServer() override;

protected:
    bool isConnected() const override;

private slots:
    void timeout();
    void modelMonitored(bool monitored = false);

private:
    void connectModel();
    void disconnectModel();

    QTimer *m_timer;
    bool m_monitored;
};
}

#endif // GAMMARAY_SELECTIONMODELSERVER_H
