/*
  attribute.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_ATTRIBUTE_H
#define GAMMARAY_ATTRIBUTE_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QAttribute>
namespace Qt3DGeometry = Qt3DCore;
#else
#include <Qt3DRender/QAttribute>
namespace Qt3DGeometry = Qt3DRender;
#endif

namespace GammaRay {
/** Utilities for decoding attribute data. */
namespace Attribute {
int size(Qt3DGeometry::QAttribute::VertexBaseType type);

QVariant variant(Qt3DGeometry::QAttribute::VertexBaseType type, const char *data);
}
}

#endif // GAMMARAY_ATTRIBUTE_H
