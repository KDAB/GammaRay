/*
  modelinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "modelinspectorinterface.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>

using namespace GammaRay;

namespace GammaRay {
GAMMARAY_ENUM_STREAM_OPERATORS(Qt::ItemFlags)

QDataStream &operator<<(QDataStream &out, const ModelCellData &data)
{
    out << data.row << data.column << data.internalId << data.internalPtr << data.flags;
    return out;
}

QDataStream &operator>>(QDataStream &in, ModelCellData &data)
{
    in >> data.row >> data.column >> data.internalId >> data.internalPtr >> data.flags;
    return in;
}
}

bool ModelCellData::operator==(const ModelCellData &other) const
{
    return row == other.row && column == other.column && internalId == other.internalId && internalPtr == other.internalPtr && flags == other.flags;
}


ModelInspectorInterface::ModelInspectorInterface(QObject *parent)
    : QObject(parent)
{
    StreamOperators::registerOperators<ModelCellData>();
    ObjectBroker::registerObject<ModelInspectorInterface *>(this);
}

ModelInspectorInterface::~ModelInspectorInterface() = default;

ModelCellData ModelInspectorInterface::currentCellData() const
{
    return m_currentCellData;
}

void ModelInspectorInterface::setCurrentCellData(const ModelCellData &cellData)
{
    if (m_currentCellData == cellData)
        return;
    m_currentCellData = cellData;
    emit currentCellDataChanged();
}
