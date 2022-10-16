/*
  connectionsextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H
#define GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {
/** Communication interface for the connections tab in the property view. */
class ConnectionsExtensionInterface : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionsExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~ConnectionsExtensionInterface() override;

    QString name() const;

public slots:
    virtual void navigateToSender(int modelRow) = 0;
    virtual void navigateToReceiver(int modelRow) = 0;

private:
    QString m_name;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ConnectionsExtensionInterface,
                    "com.kdab.GammaRay.ConnectionsExtensionInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H
