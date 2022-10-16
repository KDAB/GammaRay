/*
  networksupportinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Shantanu Tushar <shantanu.tushar@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_NETWORKSUPPORTINTERFACE_H
#define GAMMARAY_NETWORKSUPPORTINTERFACE_H

#include <QObject>

namespace GammaRay {
class NetworkSupportInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool captureResponse MEMBER m_captureResponse NOTIFY captureResponseChanged)
public:
    explicit NetworkSupportInterface(QObject *parent = nullptr);
    ~NetworkSupportInterface() override;

signals:
    void captureResponseChanged(bool captureResponse);

private:
    bool m_captureResponse = false;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::NetworkSupportInterface, "com.kdab.GammaRay.NetworkSupportInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_NETWORKSUPPORTINTERFACE_H
