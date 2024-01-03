/*
  resourceinfo.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
