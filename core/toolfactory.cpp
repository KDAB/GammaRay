/*
  toolfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
