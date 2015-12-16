/*
  paintbuffermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <config-gammaray.h>
#ifdef HAVE_PRIVATE_QT_HEADERS
#include "paintbuffermodel.h"

using namespace GammaRay;

struct cmd_t {
  QPaintBufferPrivate::Command cmd;
  const char *name;
};

#define CMD(cmd) { QPaintBufferPrivate::Cmd_ ## cmd, #cmd }

static const cmd_t cmdTypes[] =  {
  CMD(Save),
  CMD(Restore),
  CMD(SetBrush),
  CMD(SetBrushOrigin),
  CMD(SetClipEnabled),
  CMD(SetCompositionMode),
  CMD(SetOpacity),
  CMD(SetPen),
  CMD(SetRenderHints),
  CMD(SetTransform),
  CMD(SetBackgroundMode),
  CMD(ClipPath),
  CMD(ClipRect),
  CMD(ClipRegion),
  CMD(ClipVectorPath),
  CMD(DrawVectorPath),
  CMD(FillVectorPath),
  CMD(StrokeVectorPath),
  CMD(DrawConvexPolygonF),
  CMD(DrawConvexPolygonI),
  CMD(DrawEllipseF),
  CMD(DrawEllipseI),
  CMD(DrawLineF),
  CMD(DrawLineI),
  CMD(DrawPath),
  CMD(DrawPointsF),
  CMD(DrawPointsI),
  CMD(DrawPolygonF),
  CMD(DrawPolygonI),
  CMD(DrawPolylineF),
  CMD(DrawPolylineI),
  CMD(DrawRectF),
  CMD(DrawRectI),
  CMD(FillRectBrush),
  CMD(FillRectColor),
  CMD(DrawText),
  CMD(DrawTextItem),
  CMD(DrawImagePos),
  CMD(DrawImageRect),
  CMD(DrawPixmapPos),
  CMD(DrawPixmapRect),
  CMD(DrawTiledPixmap),
  CMD(SystemStateChanged),
  CMD(Translate),
  CMD(DrawStaticText)
};

class PaintBufferPrivacyViolater : public QPainterReplayer
{
  public:
    QPaintBufferPrivate *extract() const { return d; }
};

PaintBufferModel::PaintBufferModel(QObject *parent)
  : QAbstractTableModel(parent), m_privateBuffer(0)
{
}

void PaintBufferModel::setPaintBuffer(const QPaintBuffer &buffer)
{
  beginResetModel();
  m_buffer = buffer;
  PaintBufferPrivacyViolater p;
  p.processCommands(buffer, 0, 0, -1); // end < begin -> no processing
  m_privateBuffer = p.extract();
  endResetModel();
}

QPaintBuffer PaintBufferModel::buffer() const
{
  return m_buffer;
}

QVariant PaintBufferModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || !m_privateBuffer) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    const QPaintBufferCommand cmd = m_privateBuffer->commands.at(index.row());
    switch (index.column()) {
    case 0:
      return cmdTypes[cmd.id].name;
    case 1:
    {
#ifndef QT_NO_DEBUG_STREAM
      QString desc = m_buffer.commandDescription(index.row());
      const QString prefix = QLatin1String("Cmd_") + QLatin1String(cmdTypes[cmd.id].name);

      if (desc.startsWith(prefix)) {
        desc = desc.mid(prefix.length());
      }

      if (desc.startsWith(QLatin1String(": ")) || desc.startsWith(QLatin1String(", "))) {
        desc = desc.mid(2);
      }
      return desc;
#endif
    }
    }
  }

  return QVariant();
}

int PaintBufferModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
#ifndef QT_NO_DEBUG_STREAM
  return 2;
#else
  return 1;
#endif
}

int PaintBufferModel::rowCount(const QModelIndex &parent) const
{
  if (!m_privateBuffer || parent.isValid()) {
    return 0;
  }
  return m_privateBuffer->commands.size();
}

QVariant PaintBufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Command");
    case 1:
      return tr("Arguments");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#endif
