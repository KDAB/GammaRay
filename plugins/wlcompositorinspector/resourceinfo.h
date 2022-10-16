/*
  resourceinfo.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_RESOURCEINFO_H
#define GAMMARAY_RESOURCEINFO_H

#include <QCoreApplication>
#include <QStringList>

struct wl_resource;
struct wl_interface;

namespace GammaRay {

class ResourceInfo
{
    Q_DECLARE_TR_FUNCTIONS(GammaRay::ResourceInfo)
public:
    explicit ResourceInfo(wl_resource *resource);

    uint32_t id() const;
    const char *interfaceName() const;
    bool isInterface(const wl_interface *iface) const;
    QString name() const;
    QString info() const;
    QStringList infoLines() const;

private:
    wl_resource *m_resource;
};

}

#endif
