/*
  variantwrapper.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_VARIANTWRAPPER_H
#define GAMMARAY_VARIANTWRAPPER_H

#include <QDataStream>
#include <QVariant>

namespace GammaRay {
/** Transport wrapper for variants that shall not be unpacked by GammaRay::MethodArgument. */
class VariantWrapper
{
public:
    inline VariantWrapper()
    {
    }
    explicit inline VariantWrapper(const QVariant &variant)
        : m_variant(variant)
    {
    }

    inline QVariant variant() const
    {
        return m_variant;
    }
    inline void setVariant(const QVariant &v)
    {
        m_variant = v;
    }
    inline operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

private:
    QVariant m_variant;
};

inline QDataStream &operator<<(QDataStream &out, const VariantWrapper &wrapper)
{
    out << wrapper.variant();
    return out;
}

inline QDataStream &operator>>(QDataStream &in, VariantWrapper &value)
{
    QVariant v;
    in >> v;
    value.setVariant(v);
    return in;
}
}

Q_DECLARE_METATYPE(GammaRay::VariantWrapper)

#endif
