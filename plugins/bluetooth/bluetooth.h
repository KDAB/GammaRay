/*
  bluetooth.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_BLUETOOTH_BLUETOOTH_H
#define GAMMARAY_BLUETOOTH_BLUETOOTH_H

#include <core/toolfactory.h>

namespace GammaRay {
class Bluetooth : public QObject
{
    Q_OBJECT
public:
    explicit Bluetooth(Probe *probe, QObject *parent = nullptr);
};

class BluetoothFactory : public QObject, public StandardToolFactory<QObject, Bluetooth>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_bluetooth.json")

public:
    explicit BluetoothFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
