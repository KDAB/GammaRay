/*
  paintbuffermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/enumutil.h>
#include <core/varianthandler.h>

#include <common/metatypedeclarations.h>
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::BGMode)
Q_DECLARE_METATYPE(Qt::ClipOperation)
#endif

#ifdef HAVE_PRIVATE_QT_HEADERS
#include "paintbuffermodel.h"

using namespace GammaRay;

struct cmd_t {
    QPaintBufferPrivate::Command cmd;
    const char *name;
};

#define CMD(cmd) { QPaintBufferPrivate::Cmd_ ## cmd, #cmd }

static const cmd_t cmdTypes[] = {
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
    : QAbstractTableModel(parent)
    , m_privateBuffer(nullptr)
{
}

void PaintBufferModel::setPaintBuffer(const QPaintBuffer &buffer)
{
    beginResetModel();
    m_buffer = buffer;
    PaintBufferPrivacyViolater p;
    p.processCommands(buffer, nullptr, 0, -1); // end < begin -> no processing
    m_privateBuffer = p.extract();
    endResetModel();
}

QPaintBuffer PaintBufferModel::buffer() const
{
    return m_buffer;
}

template <typename T, typename Data>
static QString geometryListToString(const Data *data, int offset, int size)
{
    QStringList l;
    l.reserve(size);
    auto *elem = reinterpret_cast<const T*>(data + offset);
    for (int i = 0; i < size; ++i)
        l.push_back(VariantHandler::displayString(*elem++));
    return l.join(QLatin1String("; "));
}

QString PaintBufferModel::argumentDisplayString(const QPaintBufferCommand &cmd) const
{
    // TODO complete this to eventually replace m_buffer.commandDescription()
    switch (cmd.id) {
        case QPaintBufferPrivate::Cmd_Save:
        case QPaintBufferPrivate::Cmd_Restore:
            break;

        case QPaintBufferPrivate::Cmd_SetBrush:
        case QPaintBufferPrivate::Cmd_SetBrushOrigin:
        case QPaintBufferPrivate::Cmd_SetClipEnabled:
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_SetCompositionMode:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<QPainter::CompositionMode>(cmd.extra)));
        case QPaintBufferPrivate::Cmd_SetOpacity:
            return QString::number(m_privateBuffer->variants.at(cmd.offset).toDouble());
        case QPaintBufferPrivate::Cmd_SetPen:
            // TODO: our pen to string code lacks details compared to the old method
            //return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
            break;

        case QPaintBufferPrivate::Cmd_SetRenderHints:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<QPainter::RenderHints>(cmd.extra)));
        case QPaintBufferPrivate::Cmd_SetTransform:
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_SetBackgroundMode:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::BGMode>(cmd.extra)));

        case QPaintBufferPrivate::Cmd_ClipPath:
            break; // TODO
        case QPaintBufferPrivate::Cmd_ClipRect:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::ClipOperation>(cmd.extra)))
                + QLatin1String(": ")
                + VariantHandler::displayString(QRect(
                    QPoint(m_privateBuffer->ints.at(cmd.offset), m_privateBuffer->ints.at(cmd.offset + 1)),
                    QPoint(m_privateBuffer->ints.at(cmd.offset + 2), m_privateBuffer->ints.at(cmd.offset + 3))
                ));
        case QPaintBufferPrivate::Cmd_ClipRegion:
            break; // TODO
        case QPaintBufferPrivate::Cmd_ClipVectorPath:
            break; // TODO

        case QPaintBufferPrivate::Cmd_DrawVectorPath:
            break; // TODO
        case QPaintBufferPrivate::Cmd_FillVectorPath:
            break; // TODO
        case QPaintBufferPrivate::Cmd_StrokeVectorPath:
            break; // TODO

        case QPaintBufferPrivate::Cmd_DrawEllipseF:
            return VariantHandler::displayString(*reinterpret_cast<const QRectF*>(m_privateBuffer->floats.constData() + cmd.offset));
        case QPaintBufferPrivate::Cmd_DrawEllipseI:
            return VariantHandler::displayString(*reinterpret_cast<const QRect*>(m_privateBuffer->ints.constData() + cmd.offset));
        case QPaintBufferPrivate::Cmd_DrawLineF:
            return geometryListToString<QLineF>(m_privateBuffer->floats.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawLineI:
            return geometryListToString<QLine>(m_privateBuffer->ints.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawPath:
            break; // TODO
        case QPaintBufferPrivate::Cmd_DrawPolygonF:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<QPaintEngine::PolygonDrawMode>(cmd.extra)))
                + QLatin1String(": ") +
                geometryListToString<QPointF>(m_privateBuffer->floats.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawPolygonI:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<QPaintEngine::PolygonDrawMode>(cmd.extra)))
                + QLatin1String(": ") +
                geometryListToString<QPoint>(m_privateBuffer->ints.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawPointsF:
        case QPaintBufferPrivate::Cmd_DrawConvexPolygonF:
        case QPaintBufferPrivate::Cmd_DrawPolylineF:
            return geometryListToString<QPointF>(m_privateBuffer->floats.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawPointsI:
        case QPaintBufferPrivate::Cmd_DrawConvexPolygonI:
        case QPaintBufferPrivate::Cmd_DrawPolylineI:
            return geometryListToString<QPoint>(m_privateBuffer->ints.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawRectF:
            return geometryListToString<QRectF>(m_privateBuffer->floats.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawRectI:
            return geometryListToString<QRect>(m_privateBuffer->ints.constData(), cmd.offset, cmd.size);

        case QPaintBufferPrivate::Cmd_FillRectBrush:
        case QPaintBufferPrivate::Cmd_FillRectColor:
            return VariantHandler::displayString(*reinterpret_cast<const QRectF*>(m_privateBuffer->floats.constData() + cmd.offset))
                + QLatin1String(" - ") + VariantHandler::displayString(m_privateBuffer->variants.at(cmd.extra));

        case QPaintBufferPrivate::Cmd_DrawText:
        {
            QPointF pos(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1));
            const auto vl = m_privateBuffer->variants.at(cmd.offset).value<QList<QVariant> >();
            return tr("position: %1, text: \"%2\", font: %3").arg(
                VariantHandler::displayString(pos),
                vl.at(1).toString(), VariantHandler::displayString(vl.at(0)));
        }
        case QPaintBufferPrivate::Cmd_DrawTextItem:
            break; // TODO

        case QPaintBufferPrivate::Cmd_DrawImagePos:
        case QPaintBufferPrivate::Cmd_DrawPixmapPos:
            return VariantHandler::displayString(QPointF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra+1)));
        case QPaintBufferPrivate::Cmd_DrawImageRect:
        case QPaintBufferPrivate::Cmd_DrawPixmapRect:
        {
            QRectF r(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1),
                     m_privateBuffer->floats.at(cmd.extra + 2), m_privateBuffer->floats.at(cmd.extra + 3));
            QRectF sr(m_privateBuffer->floats.at(cmd.extra + 4), m_privateBuffer->floats.at(cmd.extra + 5),
                      m_privateBuffer->floats.at(cmd.extra + 6), m_privateBuffer->floats.at(cmd.extra + 7));
            return tr("%1 source: %2").arg(VariantHandler::displayString(r), VariantHandler::displayString(sr));
        }
        case QPaintBufferPrivate::Cmd_DrawTiledPixmap:
        {
            QRectF r(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1),
                     m_privateBuffer->floats.at(cmd.extra + 2), m_privateBuffer->floats.at(cmd.extra + 3));
            QPointF offset(m_privateBuffer->floats.at(cmd.extra + 4), m_privateBuffer->floats.at(cmd.extra + 5));
            return tr("%1 offset: %2").arg(VariantHandler::displayString(r), VariantHandler::displayString(offset));
        }

        case QPaintBufferPrivate::Cmd_SystemStateChanged:
            break; // TODO
        case QPaintBufferPrivate::Cmd_Translate:
            return VariantHandler::displayString(QPointF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1)));
        case QPaintBufferPrivate::Cmd_DrawStaticText:
            break; // TODO
    }
    return QString();
}

QVariant PaintBufferModel::argumentDecoration(const QPaintBufferCommand &cmd) const
{
    switch (cmd.id) {
        case QPaintBufferPrivate::Cmd_SetBrush:
        case QPaintBufferPrivate::Cmd_SetPen:
        case QPaintBufferPrivate::Cmd_DrawPixmapRect:
        case QPaintBufferPrivate::Cmd_DrawPixmapPos:
        case QPaintBufferPrivate::Cmd_DrawTiledPixmap:
        case QPaintBufferPrivate::Cmd_DrawImageRect:
        case QPaintBufferPrivate::Cmd_DrawImagePos:
            return VariantHandler::decoration(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_FillRectBrush:
        case QPaintBufferPrivate::Cmd_FillVectorPath:
        case QPaintBufferPrivate::Cmd_StrokeVectorPath:
        case QPaintBufferPrivate::Cmd_FillRectColor:
            return VariantHandler::decoration(m_privateBuffer->variants.at(cmd.extra));
    }
    return QVariant();
}

QVariant PaintBufferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_privateBuffer)
        return QVariant();

    const auto cmd = m_privateBuffer->commands.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return cmdTypes[cmd.id].name;
        case 1:
        {
            auto desc = argumentDisplayString(cmd);
            if (!desc.isEmpty())
                return desc;
#ifndef QT_NO_DEBUG_STREAM
            desc = m_buffer.commandDescription(index.row());
            const QString prefix = QLatin1String("Cmd_") + QLatin1String(cmdTypes[cmd.id].name);

            if (desc.startsWith(prefix))
                desc = desc.mid(prefix.length());

            if (desc.startsWith(QLatin1String(": ")) || desc.startsWith(QLatin1String(", ")))
                desc = desc.mid(2);
            return desc;
#endif
        }
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == 1) {
            return argumentDecoration(cmd);
        }
    } else if (role == Qt::EditRole && index.column() == 1) { // for fancy matrix rendering in the client
        if (cmd.id == QPaintBufferPrivate::Cmd_SetTransform)
            return m_privateBuffer->variants.at(cmd.offset);
    }

    return QVariant();
}

int PaintBufferModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int PaintBufferModel::rowCount(const QModelIndex &parent) const
{
    if (!m_privateBuffer || parent.isValid())
        return 0;
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
