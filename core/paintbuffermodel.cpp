/*
  paintbuffermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "paintbuffermodel.h"

#include <core/enumutil.h>
#include <core/varianthandler.h>

#include <common/metatypedeclarations.h>
#include <common/paintbuffermodelroles.h>

#include <algorithm>
#include <limits>
#include <vector>

#include <private/qvectorpath_p.h>
#include <private/qpainterpath_p.h>

using namespace GammaRay;

static const char* cmd_argument_names[] = {
    "position", // DrawText
    "text",
    "font",
    "position", // DrawImagePos
    "image",
    "target", // DrawImageRect
    "image",
    "source",
    "position", // DrawPixmapPos
    "pixmap",
    "target", // DrawPixmapRect
    "pixmap",
    "source",
    "rectangle", // DrawTiledPixmap
    "pixmap",
    "position",
};

struct cmd_t {
    QPaintBufferPrivate::Command cmd;
    const char *name;
    int argumentCount;
    int argumentName;
};

#define CMD(cmd) QPaintBufferPrivate::Cmd_ ## cmd, #cmd

static const cmd_t cmdTypes[] = {
    { CMD(Save), 0, 0 },
    { CMD(Restore), 0, 0 },
    { CMD(SetBrush), 0, 0 },
    { CMD(SetBrushOrigin), 0, 0 },
    { CMD(SetClipEnabled), 0, 0 },
    { CMD(SetCompositionMode), 0, 0 },
    { CMD(SetOpacity), 0, 0 },
    { CMD(SetPen), 0, 0 },
    { CMD(SetRenderHints), 0, 0 },
    { CMD(SetTransform), 0, 0 },
    { CMD(SetBackgroundMode), 0, 0 },
    { CMD(ClipPath), 0, 0 },
    { CMD(ClipRect), 0, 0 },
    { CMD(ClipRegion), 0, 0 },
    { CMD(ClipVectorPath), 0, 0 },
    { CMD(DrawVectorPath), 0, 0 },
    { CMD(FillVectorPath), 0, 0 },
    { CMD(StrokeVectorPath), 0, 0 },
    { CMD(DrawConvexPolygonF), 0, 0 },
    { CMD(DrawConvexPolygonI), 0, 0 },
    { CMD(DrawEllipseF), 0, 0 },
    { CMD(DrawEllipseI), 0, 0 },
    { CMD(DrawLineF), 0, 0 },
    { CMD(DrawLineI), 0, 0 },
    { CMD(DrawPath), 0, 0 },
    { CMD(DrawPointsF), 0, 0 },
    { CMD(DrawPointsI), 0, 0 },
    { CMD(DrawPolygonF), 0, 0 },
    { CMD(DrawPolygonI), 0, 0 },
    { CMD(DrawPolylineF), 0, 0 },
    { CMD(DrawPolylineI), 0, 0 },
    { CMD(DrawRectF), 0, 0 },
    { CMD(DrawRectI), 0, 0 },
    { CMD(FillRectBrush), 0, 0 },
    { CMD(FillRectColor), 0, 0 },
    { CMD(DrawText), 3, 0 },
    { CMD(DrawTextItem), 0, 0 },
    { CMD(DrawImagePos), 2, 3 },
    { CMD(DrawImageRect), 3, 5 },
    { CMD(DrawPixmapPos), 2, 8 },
    { CMD(DrawPixmapRect), 3, 10 },
    { CMD(DrawTiledPixmap), 3, 13 },
    { CMD(SystemStateChanged), 0, 0 },
    { CMD(Translate), 0, 0 },
    { CMD(DrawStaticText), 0, 0 }
};

#undef CMD

static const int TopLevelId = std::numeric_limits<int>::max();

PaintBufferModel::PaintBufferModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_privateBuffer(nullptr)
    , m_maxCost(0.0)
{
}

void PaintBufferModel::setPaintBuffer(const PaintBuffer &buffer)
{
    beginResetModel();
    m_buffer = buffer;
    m_privateBuffer = buffer.data();
    m_costs.clear();
    m_maxCost = 0.0;
    endResetModel();
}

PaintBuffer PaintBufferModel::buffer() const
{
    return m_buffer;
}

void PaintBufferModel::setCosts(const QVector<double>& costs)
{
    m_costs = costs;
    if (rowCount() > 0) {
        m_maxCost = *std::max_element(m_costs.constBegin(), m_costs.constEnd());
        emit dataChanged(index(0, 2, QModelIndex()), index(rowCount() - 1, 2, QModelIndex()));
    }
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

static QString vectorPathToString(QPaintBufferPrivate *data, const QPaintBufferCommand &cmd)
{
    QVectorPath path(
        data->floats.constData() + cmd.offset, cmd.size,
        cmd.offset2 & 0x80000000 ? nullptr : reinterpret_cast<const QPainterPath::ElementType*>(data->ints.constData() + cmd.offset2 + 1),
        *(data->ints.constData() + (cmd.offset2 & 0x7FFFFFFF))
    );
    if (path.isEmpty())
        return PaintBufferModel::tr("<empty>");
    return PaintBufferModel::tr("control rect: %1, elements: %2").arg(
        VariantHandler::displayString(path.controlPointRect()),
        QString::number(path.elementCount()));
}

QVariant PaintBufferModel::argumentAt(const QPaintBufferCommand& cmd, int index) const
{
    switch (cmd.id) {
        // single argument commands
        case QPaintBufferPrivate::Cmd_SetBrush:
        case QPaintBufferPrivate::Cmd_SetPen:
            return m_privateBuffer->variants.at(cmd.offset);
        case QPaintBufferPrivate::Cmd_FillRectBrush:
            return m_privateBuffer->variants.at(cmd.extra);

        // multi argument commands
        case QPaintBufferPrivate::Cmd_DrawText:
            switch (index) {
                case 0:
                    return QPointF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1));
                case 1:
                    return m_privateBuffer->variants.at(cmd.offset).value<QList<QVariant> >().at(1);
                case 2:
                    return m_privateBuffer->variants.at(cmd.offset).value<QList<QVariant> >().at(0);
            }
            break;
        case QPaintBufferPrivate::Cmd_DrawImagePos:
        case QPaintBufferPrivate::Cmd_DrawPixmapPos:
            switch (index) {
                case 0:
                    return QPointF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra+1));
                case 1:
                    return m_privateBuffer->variants.at(cmd.offset);
            }
            break;
        case QPaintBufferPrivate::Cmd_DrawImageRect:
        case QPaintBufferPrivate::Cmd_DrawPixmapRect:
            switch (index) {
                case 0:
                    return QRectF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1),
                                  m_privateBuffer->floats.at(cmd.extra + 2), m_privateBuffer->floats.at(cmd.extra + 3));
                case 1:
                    return m_privateBuffer->variants.at(cmd.offset);
                case 2:
                    return QRectF(m_privateBuffer->floats.at(cmd.extra + 4), m_privateBuffer->floats.at(cmd.extra + 5),
                                  m_privateBuffer->floats.at(cmd.extra + 6), m_privateBuffer->floats.at(cmd.extra + 7));
            }
            break;
        case QPaintBufferPrivate::Cmd_DrawTiledPixmap:
            switch (index) {
                case 0:
                    return QRectF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1),
                                  m_privateBuffer->floats.at(cmd.extra + 2), m_privateBuffer->floats.at(cmd.extra + 3));
                case 1:
                    return m_privateBuffer->variants.at(cmd.offset);
                case 2:
                    return QPointF(m_privateBuffer->floats.at(cmd.extra + 4), m_privateBuffer->floats.at(cmd.extra + 5));
            }
            break;
        default:
            break;
    }
    return QVariant();
}

QString PaintBufferModel::argumentDisplayString(const QPaintBufferCommand &cmd) const
{
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
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));

        case QPaintBufferPrivate::Cmd_SetRenderHints:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<QPainter::RenderHints>(cmd.extra)));
        case QPaintBufferPrivate::Cmd_SetTransform:
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_SetBackgroundMode:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::BGMode>(cmd.extra)));

        case QPaintBufferPrivate::Cmd_ClipRect:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::ClipOperation>(cmd.extra)))
                + QLatin1String(": ")
                + VariantHandler::displayString(QRect(
                    QPoint(m_privateBuffer->ints.at(cmd.offset), m_privateBuffer->ints.at(cmd.offset + 1)),
                    QPoint(m_privateBuffer->ints.at(cmd.offset + 2), m_privateBuffer->ints.at(cmd.offset + 3))
                ));
        case QPaintBufferPrivate::Cmd_ClipPath:
        case QPaintBufferPrivate::Cmd_ClipRegion:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::ClipOperation>(cmd.extra)))
                + QLatin1String(": ")
                + VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_ClipVectorPath:
            return EnumUtil::enumToString(QVariant::fromValue(static_cast<Qt::ClipOperation>(cmd.extra)))
                + QLatin1String(": ")
                + vectorPathToString(m_privateBuffer, cmd);

        case QPaintBufferPrivate::Cmd_DrawVectorPath:
            return vectorPathToString(m_privateBuffer, cmd);
        case QPaintBufferPrivate::Cmd_FillVectorPath:
            return tr("%1, brush: %2").arg(
                vectorPathToString(m_privateBuffer, cmd),
                VariantHandler::displayString(m_privateBuffer->variants.at(cmd.extra)));
        case QPaintBufferPrivate::Cmd_StrokeVectorPath:
            return tr("%1, pen: %2").arg(
                vectorPathToString(m_privateBuffer, cmd),
                VariantHandler::displayString(m_privateBuffer->variants.at(cmd.extra)));

        case QPaintBufferPrivate::Cmd_DrawEllipseF:
            return VariantHandler::displayString(*reinterpret_cast<const QRectF*>(m_privateBuffer->floats.constData() + cmd.offset));
        case QPaintBufferPrivate::Cmd_DrawEllipseI:
            return VariantHandler::displayString(*reinterpret_cast<const QRect*>(m_privateBuffer->ints.constData() + cmd.offset));
        case QPaintBufferPrivate::Cmd_DrawLineF:
            return geometryListToString<QLineF>(m_privateBuffer->floats.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawLineI:
            return geometryListToString<QLine>(m_privateBuffer->ints.constData(), cmd.offset, cmd.size);
        case QPaintBufferPrivate::Cmd_DrawPath:
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
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
            return tr("position: %1, text: \"%2\", font: %3").arg(
                VariantHandler::displayString(argumentAt(cmd, 0)),
                VariantHandler::displayString(argumentAt(cmd, 1)),
                VariantHandler::displayString(argumentAt(cmd, 2)));
        case QPaintBufferPrivate::Cmd_DrawTextItem:
        {
            QPointF pos(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1));
            auto textItem = reinterpret_cast<QTextItemIntCopy*>(m_privateBuffer->variants.at(cmd.offset).value<void*>());
            return tr("position: %1, text: \"%2\", font: %3").arg(
                VariantHandler::displayString(pos),
                (*textItem)().text(),
                VariantHandler::displayString((*textItem)().font())
            );
        }

        case QPaintBufferPrivate::Cmd_DrawImagePos:
        case QPaintBufferPrivate::Cmd_DrawPixmapPos:
            return VariantHandler::displayString(argumentAt(cmd, 0));
        case QPaintBufferPrivate::Cmd_DrawImageRect:
        case QPaintBufferPrivate::Cmd_DrawPixmapRect:
            return tr("%1 source: %2").arg(
                VariantHandler::displayString(argumentAt(cmd, 0)),
                VariantHandler::displayString(argumentAt(cmd, 2)));
        case QPaintBufferPrivate::Cmd_DrawTiledPixmap:
            return tr("%1 offset: %2").arg(
                VariantHandler::displayString(argumentAt(cmd, 0)),
                VariantHandler::displayString(argumentAt(cmd, 2)));

        case QPaintBufferPrivate::Cmd_SystemStateChanged:
            return VariantHandler::displayString(m_privateBuffer->variants.at(cmd.offset));
        case QPaintBufferPrivate::Cmd_Translate:
            return VariantHandler::displayString(QPointF(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1)));
        case QPaintBufferPrivate::Cmd_DrawStaticText:
        {
            const auto variants = m_privateBuffer->variants.at(cmd.offset).value<QVariantList>();
            return tr("glyphs: %1, font: %2").arg(
                QString::number((variants.size() - 1) / 2),
                VariantHandler::displayString(variants.at(0)));
        }
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

    if (!index.parent().isValid()) {
        const auto cmd = m_privateBuffer->commands.at(index.row());
        switch (role) {
            case Qt::DisplayRole:
                if (index.column() == 0)
                    return cmdTypes[cmd.id].name;
                else if (index.column() == 1)
                    return argumentDisplayString(cmd);
                else if (index.column() == 2 && m_costs.size() > index.row())
                    return m_costs.at(index.row());
                break;
            case Qt::DecorationRole:
                if (index.column() == 1)
                    return argumentDecoration(cmd);
                break;
            case Qt::EditRole:
                // for fancy matrix rendering in the client
                if (index.column() == 1 && cmd.id == QPaintBufferPrivate::Cmd_SetTransform)
                    return m_privateBuffer->variants.at(cmd.offset);
                break;
            case PaintBufferModelRoles::ValueRole:
                return argumentAt(cmd, 0);
            case PaintBufferModelRoles::ClipPathRole:
                return QVariant::fromValue(clipPath(index.row()));
            case PaintBufferModelRoles::MaxCostRole:
                if (index.column() == 2 && index.row() == 0)
                    return m_maxCost;
                break;
            case PaintBufferModelRoles::ObjectIdRole:
                return QVariant::fromValue(m_buffer.origin(index.row()));
        }
    } else {
        const auto cmd = m_privateBuffer->commands.at(index.internalId());
        switch (role) {
            case Qt::DisplayRole:
                if (index.column() == 0)
                    return cmd_argument_names[cmdTypes[cmd.id].argumentName + index.row()];
                else if (index.column() == 1)
                    return VariantHandler::displayString(argumentAt(cmd, index.row()));
                break;
            case PaintBufferModelRoles::ValueRole:
                return argumentAt(cmd, index.row());
        }
    }

    return QVariant();
}

QMap<int, QVariant> PaintBufferModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(PaintBufferModelRoles::MaxCostRole, data(index, PaintBufferModelRoles::MaxCostRole));
    d.insert(PaintBufferModelRoles::ObjectIdRole, data(index, PaintBufferModelRoles::ObjectIdRole));
    return d;
}

int PaintBufferModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int PaintBufferModel::rowCount(const QModelIndex &parent) const
{
    if (!m_privateBuffer)
        return 0;
    if (parent.isValid()) {
        const auto cmd = m_privateBuffer->commands.at(parent.row());
        return cmdTypes[cmd.id].argumentCount;
    }
    return m_privateBuffer->commands.size();
}

QModelIndex PaintBufferModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, TopLevelId);
    return createIndex(row, column, parent.row());
}

QModelIndex PaintBufferModel::parent(const QModelIndex& child) const
{
    if (child.internalId() == TopLevelId)
        return {};
    return createIndex(child.internalId(), 0, TopLevelId);
}

QPainterPath PaintBufferModel::clipPath(int row) const
{
    QPainterPath clip;
    QTransform t;
    std::vector<QPainterPath> clipStack;
    std::vector<QTransform> transformStack;

    for (int i = 0; i <= row; ++i) {
        const auto cmd = m_privateBuffer->commands.at(i);

        QPainterPath p;
        Qt::ClipOperation op = Qt::NoClip;
        switch (cmd.id) {
            case QPaintBufferPrivate::Cmd_Save:
                clipStack.push_back(clip);
                transformStack.push_back(t);
                continue;
            case QPaintBufferPrivate::Cmd_Restore:
                if (clipStack.empty() || transformStack.empty())
                    return QPainterPath();
                clip = clipStack.back();
                clipStack.pop_back();
                t = transformStack.back();
                transformStack.pop_back();
                continue;
            case QPaintBufferPrivate::Cmd_SetTransform:
                t = m_privateBuffer->variants.at(cmd.offset).value<QTransform>();
                continue;
            case QPaintBufferPrivate::Cmd_Translate:
                t.translate(m_privateBuffer->floats.at(cmd.extra), m_privateBuffer->floats.at(cmd.extra + 1));
                continue;
            case QPaintBufferPrivate::Cmd_ClipRect:
                p.addRect(QRect(QPoint(m_privateBuffer->ints.at(cmd.offset), m_privateBuffer->ints.at(cmd.offset + 1)),
                                QPoint(m_privateBuffer->ints.at(cmd.offset + 2), m_privateBuffer->ints.at(cmd.offset + 3))));
                p = t.map(p);
                op = static_cast<Qt::ClipOperation>(cmd.extra);
                break;
            case QPaintBufferPrivate::Cmd_ClipRegion:
                p.addRegion(m_privateBuffer->variants.at(cmd.offset).value<QRegion>());
                p = t.map(p);
                op = static_cast<Qt::ClipOperation>(cmd.extra);
                break;
            case QPaintBufferPrivate::Cmd_ClipPath:
                p = m_privateBuffer->variants.at(cmd.offset).value<QPainterPath>();
                p = t.map(p);
                op = static_cast<Qt::ClipOperation>(cmd.extra);
                break;
            case QPaintBufferPrivate::Cmd_ClipVectorPath:
                p = QVectorPath(m_privateBuffer->floats.constData() + cmd.offset, cmd.size,
                                cmd.offset2 & 0x80000000 ? nullptr : reinterpret_cast<const QPainterPath::ElementType*>(m_privateBuffer->ints.constData() + cmd.offset2 + 1),
                                *(m_privateBuffer->ints.constData() + (cmd.offset2 & 0x7FFFFFFF))).convertToPainterPath();
                p = t.map(p);
                op = static_cast<Qt::ClipOperation>(cmd.extra);
                break;
            case QPaintBufferPrivate::Cmd_SystemStateChanged:
                p.addRegion(m_privateBuffer->variants.at(cmd.offset).value<QRegion>());
                op = Qt::ReplaceClip;
                break;
            default:
                continue;
        }

        switch (op) {
            case Qt::NoClip:
                clip = QPainterPath();
                break;
            case Qt::ReplaceClip:
                clip = p;
                break;
            case Qt::IntersectClip:
                clip = clip.intersected(p);
                break;
        }
    }
    return clip;
}
