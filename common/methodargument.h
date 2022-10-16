/*
  methodargument.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METHODARGUMENT_H
#define GAMMARAY_METHODARGUMENT_H

#include "gammaray_common_export.h"
#include <QSharedDataPointer>
#include <QVariant>

namespace GammaRay {
class MethodArgumentPrivate;

/** Q[Generic]Argument that works on a QVariant, with some memory handling safety. */
class GAMMARAY_COMMON_EXPORT MethodArgument
{
public:
    MethodArgument();
    explicit MethodArgument(const QVariant &v);
    MethodArgument(const MethodArgument &other);
    ~MethodArgument();

    MethodArgument &operator=(const MethodArgument &other);
    operator QGenericArgument() const;

private:
    QExplicitlySharedDataPointer<MethodArgumentPrivate> d;
};
}

#endif // GAMMARAY_METHODARGUMENT_H
