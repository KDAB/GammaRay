/*
  toolfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "toolfactory.h"

using namespace GammaRay;

ToolFactory::ToolFactory() = default;

ToolFactory::~ToolFactory() = default;

const QVector<QByteArray> &ToolFactory::supportedTypes() const
{
    return m_types;
}

void ToolFactory::setSupportedTypes(const QVector<QByteArray> &types)
{
    m_types = types;
}

QString ToolFactory::supportedTypesString() const
{
    QStringList typesString;
    for (auto it = m_types.constBegin(), end = m_types.constEnd(); it != end; ++it)
        typesString << QLatin1String(*it);
    return typesString.join(QStringLiteral(", "));
}

QVector<QByteArray> ToolFactory::selectableTypes() const
{
    return QVector<QByteArray>();
}

bool ToolFactory::isHidden() const
{
    return false;
}
