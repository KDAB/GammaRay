/*
  methodsextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METHODSEXTENSIONCLIENT_H
#define GAMMARAY_METHODSEXTENSIONCLIENT_H

#include <common/tools/objectinspector/methodsextensioninterface.h>

namespace GammaRay {
class MethodsExtensionClient : public MethodsExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MethodsExtensionInterface)
public:
    explicit MethodsExtensionClient(const QString &name, QObject *parent = nullptr);
    ~MethodsExtensionClient() override;

public slots:
    void activateMethod() override;
    void invokeMethod(Qt::ConnectionType type) override;
    void connectToSignal() override;
};
}

#endif // GAMMARAY_METHODSEXTENSIONCLIENT_H
