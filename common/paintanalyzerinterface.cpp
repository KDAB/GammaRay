/*
  paintanalyzerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintanalyzerinterface.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>

using namespace GammaRay;

PaintAnalyzerInterface::PaintAnalyzerInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_hasArgumentDetails(false)
    , m_hasStackTrace(false)
{
    ObjectBroker::registerObject(name, this);
    StreamOperators::registerOperators<PaintAnalyzerFrameData>();
}

QString PaintAnalyzerInterface::name() const
{
    return m_name;
}

bool PaintAnalyzerInterface::hasArgumentDetails() const
{
    return m_hasArgumentDetails;
}

void PaintAnalyzerInterface::setHasArgumentDetails(bool hasDetails)
{
    if (m_hasArgumentDetails == hasDetails)
        return;
    m_hasArgumentDetails = hasDetails;
    emit hasArgumentDetailsChanged(hasDetails);
}

bool PaintAnalyzerInterface::hasStackTrace() const
{
    return m_hasStackTrace;
}

void PaintAnalyzerInterface::setHasStackTrace(bool hasStackTrace)
{
    if (m_hasStackTrace == hasStackTrace)
        return;
    m_hasStackTrace = hasStackTrace;
    emit hasStackTraceChanged(hasStackTrace);
}

namespace GammaRay {
QDataStream& operator<<(QDataStream &stream, const PaintAnalyzerFrameData &data)
{
    stream << data.clipPath;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, PaintAnalyzerFrameData &data)
{
    stream >> data.clipPath;
    return stream;
}
}
