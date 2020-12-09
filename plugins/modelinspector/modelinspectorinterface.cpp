/*
  modelinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

bool ModelCellData::operator==(const ModelCellData& other) const
{
    return row == other.row &&
           column == other.column &&
           internalId == other.internalId &&
           internalPtr == other.internalPtr &&
           flags == other.flags;
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

void ModelInspectorInterface::setCurrentCellData(const ModelCellData& cellData)
{
    if (m_currentCellData == cellData)
        return;
    m_currentCellData = cellData;
    emit currentCellDataChanged();
}
