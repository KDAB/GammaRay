/*
  sysinfo.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "sysinfo.h"
#include "environmentmodel.h"
#include "libraryinfomodel.h"
#include "standardpathsmodel.h"
#include "sysinfomodel.h"

using namespace GammaRay;

SysInfo::SysInfo(Probe *probe, QObject *parent)
    : QObject(parent)
{
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SysInfoModel"), new SysInfoModel(this));
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LibraryInfoModel"), new LibraryInfoModel(this));
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.EnvironmentModel"), new EnvironmentModel(this));

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.StandardPathsModel"), new StandardPathsModel(this));
}

SysInfo::~SysInfo() = default;
