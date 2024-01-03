/*
  paintanalyzerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
QDataStream &operator<<(QDataStream &stream, const PaintAnalyzerFrameData &data)
{
    stream << data.clipPath;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, PaintAnalyzerFrameData &data)
{
    stream >> data.clipPath;
    return stream;
}
}
