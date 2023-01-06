/*
  sysinfo.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SYSINFO_H
#define GAMMARAY_SYSINFO_H

#include <core/toolfactory.h>

namespace GammaRay {

class SysInfo : public QObject
{
    Q_OBJECT
public:
    explicit SysInfo(Probe *probe, QObject *parent = nullptr);
    ~SysInfo() override;

private:
};

class SysInfoFactory : public QObject, public StandardToolFactory<QObject, SysInfo>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_sysinfo.json")
public:
    explicit SysInfoFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};

}

#endif // GAMMARAY_SYSINFO_H
