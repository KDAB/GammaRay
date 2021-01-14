/*
  resourceinfo.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_RESOURCEINFO_H
#define GAMMARAY_RESOURCEINFO_H

#include <QCoreApplication>
#include <QStringList>

struct wl_resource;
struct wl_interface;

namespace GammaRay
{

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
