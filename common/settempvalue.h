/*
  settempvalue.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SETTEMPVALUE_H
#define GAMMARAY_SETTEMPVALUE_H

#include <qglobal.h>

namespace GammaRay {
/**
 * @brief GammaRay utilities.
 */
namespace Util {
/**
 * Temporarily overwrite a given object with a new value and reset the value
 * when the scope is exited.
 */
template<class T>
struct SetTempValue
{
    SetTempValue(T &obj, T newValue)
        : obj(obj)
        , oldValue(obj)
    {
        obj = std::move(newValue);
    }

    ~SetTempValue()
    {
        obj = oldValue;
    }

    Q_DISABLE_COPY(SetTempValue)
    T &obj;
    T oldValue;
};
}
}

#endif // GAMMARAY_SETTEMPVALUE_H
