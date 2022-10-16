/*
  connectionstab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CONNECTIONSTAB_H
#define GAMMARAY_CONNECTIONSTAB_H

#include <QWidget>

namespace GammaRay {
class ConnectionsExtensionInterface;
class PropertyWidget;
namespace Ui {
class ConnectionsTab;
}

class ConnectionsTab : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionsTab(PropertyWidget *parent = nullptr);
    ~ConnectionsTab() override;

private slots:
    void inboundContextMenu(const QPoint &pos);
    void outboundContextMenu(const QPoint &pos);

private:
    QScopedPointer<Ui::ConnectionsTab> ui;
    ConnectionsExtensionInterface *m_interface;
};
}

#endif // GAMMARAY_CONNECTIONSTAB_H
