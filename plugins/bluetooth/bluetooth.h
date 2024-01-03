/*
  bluetooth.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
