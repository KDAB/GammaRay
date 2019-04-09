/*
  resourceinfo.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "resourceinfo.h"

#include <functional>

#include <QWaylandSurface>
#include <QWaylandWlShellSurface>
#include <QWaylandOutput>
#include <QMetaEnum>

#include <wayland-server.h>

namespace GammaRay
{

class ResourceInfoExtractors
{
    Q_DECLARE_TR_FUNCTIONS(GammaRay::ResourceInfoExtractors)
public:
  using Function = std::function<void (wl_resource *, QStringList &)>;

  ResourceInfoExtractors()
  {
    m_infoExtractors[wl_surface_interface.name] = wlsurfaceInfo;
    m_infoExtractors[wl_shell_surface_interface.name] = wlshellsurfaceInfo;
    m_infoExtractors[wl_output_interface.name] = wloutputInfo;
  }

  Function extractor(wl_resource *res) const
  {
    return m_infoExtractors.value(wl_resource_get_class(res));
  }

  static void wlsurfaceInfo(wl_resource *res, QStringList &lines)
  {
    QWaylandSurface *surface = QWaylandSurface::fromResource(res);
    lines << tr("Role: %1").arg(surface->role() ? QString(surface->role()->name()) : QStringLiteral("none"));
    lines << tr("Buffer size: (%1x%2)").arg(QString::number(surface->size().width()), QString::number(surface->size().height()));
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    lines << tr("Is mapped: %1").arg(surface->isMapped() ? QStringLiteral("true") : QStringLiteral("false"));
#else
    lines << tr("Has content: %1").arg(surface->hasContent() ? QStringLiteral("true") : QStringLiteral("false"));
#endif
  }
  static void wlshellsurfaceInfo(wl_resource *res, QStringList &lines)
  {
    QWaylandWlShellSurface *ss = QWaylandWlShellSurface::fromResource(res);
    if (!ss) {
      return;
    }

    lines << tr("Title: \"%1\"").arg(ss->title());
    lines << tr("Class name: \"%1\"").arg(ss->className());

    ResourceInfo resinfo(ss->surface()->resource());
    lines << tr("Surface: %1").arg(resinfo.name());
    foreach (const QString &line, resinfo.infoLines()) {
      lines << QStringLiteral("   ") + line;
    }
  }
  static void wloutputInfo(wl_resource *res, QStringList &lines)
  {
    QWaylandOutput *output = QWaylandOutput::fromResource(res);

    lines << tr("Manufacturer: %1").arg(output->manufacturer());
    lines << tr("Model: %1").arg(output->model());
    lines << tr("Physical size: (%1x%2)").arg(QString::number(output->physicalSize().width()), QString::number(output->physicalSize().height()));
    lines << tr("Position: (%1x%2)").arg(QString::number(output->position().x()), QString::number(output->position().y()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    const auto mode = output->currentMode();
    lines << tr("Current mode: %1x%2@%3Hz").arg(QString::number(mode.size().width()), QString::number(mode.size().height()), QString::number(mode.refreshRate() / 1000.0f));
#endif
    lines << tr("Scale factor: %1").arg(QString::number(output->scaleFactor()));
    lines << tr("Transform: %1").arg(QMetaEnum::fromType<QWaylandOutput::Transform>().valueToKey(output->transform()));
    lines << tr("Subpixel: %1").arg(QMetaEnum::fromType<QWaylandOutput::Subpixel>().valueToKey(output->subpixel()));
  }

  QHash<QByteArray, Function> m_infoExtractors;
};

Q_GLOBAL_STATIC(ResourceInfoExtractors, s_infoExtractors)

ResourceInfo::ResourceInfo(wl_resource *resource)
            : m_resource(resource)
{
}

uint32_t ResourceInfo::id() const
{
  return wl_resource_get_id(m_resource);
}

const char *ResourceInfo::interfaceName() const
{
  return wl_resource_get_class(m_resource);
}

bool ResourceInfo::isInterface(const wl_interface *iface) const
{
  return strcmp(interfaceName(), iface->name) == 0;
}

QString ResourceInfo::name() const
{
  return QString("%1@%2").arg(interfaceName(), QString::number(id()));
}

QString ResourceInfo::info() const
{
  const QStringList lines = infoLines();
  QString str;
  for (const QString &line : lines) {
    if (!str.isEmpty()) {
      str += QLatin1Char('\n');
    }
    str += line;
  }
  return str;
}

QStringList ResourceInfo::infoLines() const
{
  QStringList lines;
  lines << tr("Version: %1").arg(QString::number(wl_resource_get_version(m_resource)));

  if (const auto &func = s_infoExtractors->extractor(m_resource)) {
    func(m_resource, lines);
  }

  return lines;
}

}
