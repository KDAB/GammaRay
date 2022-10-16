/*
  attribute.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ATTRIBUTE_H
#define GAMMARAY_ATTRIBUTE_H

#include <Qt3DRender/QAttribute>

namespace GammaRay {
/** Utilities for decoding attribute data. */
namespace Attribute {
int size(Qt3DRender::QAttribute::VertexBaseType type);

QVariant variant(Qt3DRender::QAttribute::VertexBaseType type, const char *data);
}
}

#endif // GAMMARAY_ATTRIBUTE_H
