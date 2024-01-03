/*
  methodargument.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
